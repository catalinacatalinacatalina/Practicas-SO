#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <errno.h>

pid_t run_command(const char* command) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Proceso hijo
        execl("/bin/sh", "sh", "-c", command, (char*) NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    // Proceso padre retorna el PID del hijo
    return pid;
}



void execute_commands_from_file(const char *filename, int concurrente) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    pid_t pids[1024];
    int command_count = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        printf("@@ Running command #%d: %s\n", command_count, line);

        pids[command_count] = run_command(line);

        if(!concurrente){
            int status;
            waitpid(pids[command_count], &status, 0);
            printf("@@ Command #%d terminated (pid: %d, status: %d)\n", command_count, pids[command_count], WEXITSTATUS(status));
        }

        command_count++;
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    pid_t pid;
    int opt;
    char *single_command = NULL;
    char* route =NULL;
    int concurrente =0;
    while ((opt = getopt(argc, argv, "x:s:b")) != -1) {
        switch (opt) {
            case 'x':
                single_command = optarg;
                break;
            case 's':
                route = optarg;
                break;
            case 'b':
                concurrente=1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-x command] [-s file] [-b]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
        
    if (single_command) {
         pid = run_command(single_command);
        if (pid < 0) {
            fprintf(stderr, "Error ejecutando comando\n");
            exit(1);
        }

        waitpid(pid, NULL, 0);
    }

    if(route){
        execute_commands_from_file(route, concurrente);
    }

    return EXIT_SUCCESS;
}
