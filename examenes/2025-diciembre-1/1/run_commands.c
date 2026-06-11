#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <errno.h>

pid_t launch_command_redirection(char* cmd, char* logfile_path){
    // eliminar saltos de linea y &
    char cmd_clean[2048];
    strcpy(cmd_clean, cmd);
    for (int i =0; i<strlen(cmd_clean);i++) {
        if(cmd_clean[i] == '\n' || cmd_clean[i] == '&'){
            cmd_clean[i] = ' ';
        }
    }
    // construir array argv con 3 elems
    char full_cmd[2048];
    sprintf(full_cmd, "%s >> %s", cmd_clean, logfile_path);

    char* argv [4];
    argv[0] = "/bin/bash";
    argv[1] = "-c";
    argv[2] = full_cmd;
    argv[3] = NULL;
    // crear proceso hijo
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // proceso hijo
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    // proceso padre: retorna el pid del hijo
    return pid;
}

// crea un proceso hijo que ejecuta el comando pasado en argv
pid_t launch_command(char** argv) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // proceso hijo: reemplaza su imagen por el comando
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    // proceso padre: retorna el pid del hijo
    return pid;
}

// parsea una linea de comando en argumentos separados por espacios
// retorna un array de strings (argv) y la cantidad de argumentos
char **parse_command(const char *cmd, int *argc) {
    size_t argv_size = 10;
    const char *end;
    size_t arg_len;
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++;

    while (*start) {
        if (arg_count >= argv_size - 1) {
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        end = start;
        while (*end && !isspace(*end)) end++;

        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';
        arg_count++;

        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL;
    (*argc) = arg_count;

    return argv;
}

// lee comandos del fichero y los ejecuta
// si run_in_background es 0: espera a que terminen (secuencial)
// si run_in_background es 1: lanza todos y espera al final (paralelo)
void execute_commands_from_file(const char *filename, int run_in_background, int segmentar, char* file_log) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    pid_t pids[1024];
    int command_count = 0;
    pid_t pids_no_ampersand[1024];
    int pids_na_ctr =0;

    // lee cada linea del fichero
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        
        // miro si tiene ampersand y si lo tiene lo borro
        int has_ampersand = 0;
        if (line[strlen(line)-1] == '&') {
            has_ampersand = 1;
            line[strlen(line)-1] = '\0';
        }

        int argc;
        // parsea la linea en argumentos
        char **argv = parse_command(line, &argc);

        printf("@@ Running command #%d: %s\n", command_count, line);

        // lanza el comando
        if(segmentar && has_ampersand){
            if(file_log == NULL)
                pids_no_ampersand[command_count] = launch_command(argv);
            else
                pids_no_ampersand[command_count] = launch_command_redirection(line, file_log);    
            pids_na_ctr++;
        }else{
            if(file_log == NULL)
                pids[command_count] = launch_command(argv);
            else
                pids[command_count] = launch_command_redirection(line, file_log);
                
            // modo secuencial: espera inmediatamente
            if (!run_in_background || (segmentar && !has_ampersand)) {
                int status;
                waitpid(pids[command_count], &status, 0);
                printf("@@ Command #%d terminated (pid: %d, status: %d)\n", command_count, pids[command_count], WEXITSTATUS(status));
            }
        }

        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);

        command_count++;
    }

    // modo paralelo: espera a todos los procesos al final
    if (run_in_background ) {
        for (int i = 0; i < command_count; i++) {
            int status;
            pid_t pid = waitpid(pids[i], &status, 0);
            printf("@@ Command #%d terminated (pid: %d, status: %d)\n", i, pid, WEXITSTATUS(status));
        }
    }

    if (segmentar){
        for(int i = 0; i<pids_na_ctr; i++){
            int status;
            pid_t pid = waitpid(pids_no_ampersand[i], &status, 0);
            printf("@@ Command #%d terminated (pid: %d, status: %d)\n", i, pid, WEXITSTATUS(status));

        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    int opt;
    int run_in_background = 0;
    int segmentar = 0;
    char *single_command = NULL;
    char *file_with_commands = NULL;
    char* file_log = NULL;
    while ((opt = getopt(argc, argv, "x:s:bBL:")) != -1) {
        switch (opt) {
            case 'x':
                single_command = optarg;
                break;
            case 's':
                file_with_commands = optarg;
                break;
            case 'b':
                run_in_background = 1;
                break;
            case 'B':
                segmentar = 1;
                break;
            case 'L':
                file_log = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-x command] [-s file] [-b]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (single_command) {
        int cmd_argc;
        char **cmd_argv = parse_command(single_command, &cmd_argc);

        pid_t pid;
        if (file_log==NULL){
            pid = launch_command(cmd_argv);
        } else{
            pid = launch_command_redirection(single_command, file_log);
        }
        int status;
        waitpid(pid, &status, 0);
        printf("@@ Command terminated (pid: %d, status: %d)\n", pid, WEXITSTATUS(status));

        for (int i = 0; i < cmd_argc; i++) {
            free(cmd_argv[i]);
        }
        free(cmd_argv);
    }

    if (file_with_commands) {
        execute_commands_from_file(file_with_commands, run_in_background, segmentar, file_log);
    }

    return EXIT_SUCCESS;
}
