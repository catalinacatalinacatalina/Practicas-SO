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
    sem_close(mutex);
    sem_close(sem_cocinar);
    sem_close(sem_lleno);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_COCINAR);
    sem_unlink(SEM_LLENO);
    shm_unlink(SHM_NAME);
}

void handler(int signo) {
    finish = 1;
    printf("\n[COCINERO %d] Recibida señal %d, limpiando recursos y saliendo...\n", getpid(), signo);
    limpiar();
    exit(EXIT_SUCCESS);
}

void putServingsInPot() {
    printf("[COCINERO] Llenando el caldero con %d raciones.\n", M);
    *raciones = M;
    sem_post(sem_lleno);
}


int main(int argc, char *argv[]) {
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("[COCINERO] Error al crear memoria compartida");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(int)) == -1) {
        perror("[COCINERO] Error al truncar memoria compartida");
        exit(1);
    }

    raciones = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (raciones == MAP_FAILED) {
        perror("[COCINERO] Error al mapear memoria compartida");
        exit(1);
    }

    sem_cocinar = sem_open(SEM_COCINAR, O_CREAT, 0666, 0);
    sem_lleno = sem_open(SEM_LLENO, O_CREAT, 0666, 0);
    mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);

    if (sem_cocinar == SEM_FAILED || sem_lleno == SEM_FAILED || mutex == SEM_FAILED) {
        perror("[COCINERO] Error al crear semáforos");
        limpiar();
        exit(1);
    }

    printf("[COCINERO] Recursos creados. Esperando solicitudes...\n");
    while (1) {
        sem_wait(sem_cocinar);
        putServingsInPot();
    }

    return 0;
}
