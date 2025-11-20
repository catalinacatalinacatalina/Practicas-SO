#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <errno.h>

pid_t launch_command(char** argv) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Proceso hijo
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    // Proceso padre retorna el PID del hijo
    return pid;
}

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

void execute_commands_from_file(const char *filename, int run_in_background) {
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
        int argc;
        char **argv = parse_command(line, &argc);

        printf("@@ Running command #%d: %s\n", command_count, line);

        pids[command_count] = launch_command(argv);

        if (!run_in_background) {
            int status;
            waitpid(pids[command_count], &status, 0);
            printf("@@ Command #%d terminated (pid: %d, status: %d)\n", command_count, pids[command_count], WEXITSTATUS(status));
        }

        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);

        command_count++;
    }

    if (run_in_background) {
        for (int i = 0; i < command_count; i++) {
            int status;
            pid_t pid = waitpid(pids[i], &status, 0);
            printf("@@ Command #%d terminated (pid: %d, status: %d)\n", i, pid, WEXITSTATUS(status));
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    int opt;
    int run_in_background = 0;
    char *single_command = NULL;
    char *file_with_commands = NULL;

    while ((opt = getopt(argc, argv, "x:s:b")) != -1) {
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
            default:
                fprintf(stderr, "Usage: %s [-x command] [-s file] [-b]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (single_command) {
        int cmd_argc;
        char **cmd_argv = parse_command(single_command, &cmd_argc);

        pid_t pid = launch_command(cmd_argv);
        int status;
        waitpid(pid, &status, 0);
        printf("@@ Command terminated (pid: %d, status: %d)\n", pid, WEXITSTATUS(status));

        for (int i = 0; i < cmd_argc; i++) {
            free(cmd_argv[i]);
        }
        free(cmd_argv);
    }

    if (file_with_commands) {
        execute_commands_from_file(file_with_commands, run_in_background);
    }

    return EXIT_SUCCESS;
}
