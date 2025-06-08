#define _POSIX_C_SOURCE 200809L //WSL


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/types.h>


#define M 10
#define SHM_NAME "/caldero_shm"
#define SEM_MUTEX "/sem_mutex"
#define SEM_COCINAR "/sem_cocinar"
#define SEM_LLENO "/sem_lleno"

int finish = 0;
int *raciones;
int shm_fd;
sem_t *mutex, *sem_cocinar, *sem_lleno;

void limpiar() {
    munmap(raciones, sizeof(int));
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(mutex);
    sem_close(sem_cocinar);
    sem_close(sem_lleno);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_COCINAR);
    sem_unlink(SEM_LLENO);
}

void handler(int signo) {
    finish = 1;
}

void putServingsInPot(int servings) {
    // Espera a que le avisen para cocinar
    sem_wait(sem_cocinar);
    *raciones = servings;
    printf("[COCINERO %d] Relleno el caldero con %d raciones\n", getpid(), servings);
    // Despierta a los salvajes que esperan raciones
    for(int i=0; i < servings; i++) {
        sem_post(sem_lleno);
    }
}

void cook(void) {
    while(!finish) {
        putServingsInPot(M);
        usleep(100000); // Pequeño retardo para evitar busy wait
    }
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Crear memoria compartida
    if((shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, sizeof(int)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    raciones = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (raciones == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    *raciones = 0;

    // Crear semáforos (todos con valor inicial 0 menos el mutex que es 1)
    mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    sem_cocinar = sem_open(SEM_COCINAR, O_CREAT, 0666, 0);
    sem_lleno = sem_open(SEM_LLENO, O_CREAT, 0666, 0);

    printf("Caldero y semáforos creados.\nCtrl+C para limpiar y salir.\n");

    cook();

    limpiar();
    return 0;
}
