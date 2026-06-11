#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>

#define SHM_NAME "/sem_loading_docks"

sem_t* semaforo;

int main(int argc, char *argv[]){
    if(argc!=2){
        perror("uso");
        return EXIT_FAILURE;
    }
    sem_unlink(SHM_NAME);

    int n = atoi(argv[1]);
    semaforo = sem_open(SHM_NAME,  O_CREAT, 0666, n);

    if (semaforo == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    printf("System initialized with %d loading docks.\n", n);


    return EXIT_SUCCESS;
}
