#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <errno.h>


pid_t run_command(const char* command){
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if ( pid == 0 ){
        execl("/bin/sh", "sh", "-c", command, (char*) NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void leer_fichero(const char* fichero, int run_in_background){
    FILE* fp = fopen(fichero, "r");

    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    char line[1024];
    pid_t pids[1024];
    int command_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        pid_t pid = run_command(line);

        if(run_in_background) {
            pids[command_count] = pid;
        }else{
            int status;
            waitpid(pid, &status, 0);
            printf("@@ Command #%d terminated (pid: %d, status: %d)\n", command_count, pid, status);
        }
        command_count++;

    }
    
    if(run_in_background){

        for(int i =0; i<command_count; i++){
            int status;
            waitpid(pids[i], &status, 0);
            printf("@@ Command #%d terminated (pid: %d, status: %d)\n", i, pids[i], status);
            
        }
    }
}

int main(int argc, char *argv[]) {
    
    int opt;
    int run_in_background = 0;
    char *command = NULL;
    char *fichero = NULL;
    while ((opt = getopt(argc, argv, "x:s:b")) != -1) {
        switch (opt) {
            case 'x':
                command = optarg;
                break;
            case 's':
                fichero = optarg;
                break;
            case 'b':
                run_in_background = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-x command] [-s file] [-b]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if(fichero!=optarg){
        leer_fichero(fichero, run_in_background);

    }
    
    if(command!=NULL){ 
        pid_t pid = run_command(command);
        int status;
        waitpid(pid, &status, 0);
    }
}