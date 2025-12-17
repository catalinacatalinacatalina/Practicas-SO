#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include "globals.h"

#define NUMITER 3
#define SHM_NAME "/caldero_shm"
#define SEM_MUTEX "/sem_mutex"
#define SEM_COOK "/sem_cocinar"
#define SEM_SAVAGES "/sem_savages"

shared_t *shared;
sem_t *mutex, *cook_sem, *sav_sem;

void getServing(int id) {
    sem_wait(mutex);

    while (shared->servings == 0) {
        printf("[SALVAJE %d] Caldero vacío, llamando al cocinero\n", id);
        shared->sav_waiting++;
        sem_post(cook_sem);
        sem_post(mutex);
        sem_wait(sav_sem);
        sem_wait(mutex);
    }

    shared->servings--;
    printf("[SALVAJE %d] Toma ración. Quedan %d\n",
           id, shared->servings);

    sem_post(mutex);
}

void eat(int id) {
    printf("[SALVAJE %d] Comiendo...\n", id);
    sleep(rand() % 3 + 1);
}

int main() {
    srand(getpid());

    /* Memoria compartida */
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ejecuta primero el cocinero");
        exit(1);
    }

    shared = mmap(NULL, sizeof(shared_t),
                  PROT_READ | PROT_WRITE, MAP_SHARED,
                  shm_fd, 0);

    /* Semáforos */
    mutex    = sem_open(SEM_MUTEX, 0);
    cook_sem = sem_open(SEM_COOK, 0);
    sav_sem  = sem_open(SEM_SAVAGES, 0);

    if (mutex == SEM_FAILED || cook_sem == SEM_FAILED || sav_sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    int id = getpid();

    for (int i = 0; i < NUMITER; i++) {
        getServing(id);
        eat(id);
    }

    printf("[SALVAJE %d] Ha terminado\n", id);
    return 0;
}
