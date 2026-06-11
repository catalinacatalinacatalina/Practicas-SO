#define _POSIX_C_SOURCE 200809L // SOLO EN WSL
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>


int run_command(char*cmd){
    struct sigaction sa_ign, sa_old_int, sa_old_term;
    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags = 0;
    sigemptyset(&sa_ign.sa_mask);
    sigaction(SIGINT,  &sa_ign, &sa_old_int);
    sigaction(SIGTERM, &sa_ign, &sa_old_term);

    pid_t pid = fork();
    if(pid==0){
        // pongo comportamiento por defecto en hijo
        struct sigaction sa_defecto;
        sa_defecto.sa_handler = SIG_DFL; 
        sa_defecto.sa_flags = 0;
        sigemptyset(&sa_defecto.sa_mask);
        sigaction(SIGTERM, &sa_defecto, NULL);
        sigaction(SIGINT, &sa_defecto, NULL);

        execl("/bin/sh", "sh", "-c", cmd, (char*) NULL);
        perror("execvp");
    }

    printf("child process pid %d\n", pid);

    int status;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            break;
        }
    }
    
    if (WIFSIGNALED(status))
        printf("child with pid %d terminated by signal %d\n", pid, WTERMSIG(status));

    // Restaurar señales originales en el padre
    sigaction(SIGINT,  &sa_old_int,  NULL);
    sigaction(SIGTERM, &sa_old_term, NULL);



    return 0;
}

int main(int argc, char *argv[]) {
    if(argc!=2){
        perror("uso");
        return EXIT_FAILURE;
    }
    

    run_command(argv[1]);
}