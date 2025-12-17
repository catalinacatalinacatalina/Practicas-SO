#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include "globals.h"

#define M 10
#define SHM_NAME "/caldero_shm"

#define SEM_MUTEX "/sem_mutex"
#define SEM_COCINAR "/sem_cocinar"
#define SEM_SAVAGES "/sem_savages"

volatile sig_atomic_t finish = 0;

shared_t *shared;
sem_t *mutex, *cook_sem, *sav_sem;

/* ---------- señales ---------- */
void handler(int sig) {
    finish = 1;
}

/* ---------- función original (NO cambiada) ---------- */
void putServingsInPot(void) {
    /* mutex interruptible */
    while (sem_wait(mutex) == -1 && errno == EINTR) {
        if (finish) return;
    }

    /* si otro cocinero ya cocinó, dormir */
    while (shared->servings > 0 && !finish) {
        shared->cook_waiting++;
        sem_post(mutex);

        while (sem_wait(cook_sem) == -1 && errno == EINTR) {
            if (finish) return;
        }

        while (sem_wait(mutex) == -1 && errno == EINTR) {
            if (finish) return;
        }
    }

    if (finish) {
        sem_post(mutex);
        return;
    }

    /* cocinar */
    shared->servings = M;
    printf("[COCINERO %d] Caldero lleno (%d raciones)\n", getpid(), M);

    /* despertar a todos los salvajes */
    while (shared->sav_waiting > 0) {
        sem_post(sav_sem);
        shared->sav_waiting--;
    }

    /* despertar a otro cocinero si hay */
    if (shared->cook_waiting > 0) {
        shared->cook_waiting--;
        sem_post(cook_sem);
    }

    sem_post(mutex);
}

int main(void) {
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    mutex    = sem_open(SEM_MUTEX,   O_CREAT, 0666, 1);
    cook_sem = sem_open(SEM_COCINAR,    O_CREAT, 0666, 0);
    sav_sem  = sem_open(SEM_SAVAGES, O_CREAT, 0666, 0);

    if (mutex == SEM_FAILED || cook_sem == SEM_FAILED || sav_sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(shared_t));

    shared = mmap(NULL, sizeof(shared_t),
                  PROT_READ | PROT_WRITE,
                  MAP_SHARED, shm_fd, 0);

    sem_wait(mutex);
    shared->servings = 0;
    shared->sav_waiting = 0;
    shared->cook_waiting = 0;
    sem_post(mutex);

    printf("[COCINERO %d] Listo\n", getpid());

    while (!finish) {
        while (sem_wait(cook_sem) == -1 && errno == EINTR) {
            if (finish) break;
        }
        if (finish) break;

        putServingsInPot();
    }

    printf("[COCINERO %d] Finalizando\n", getpid());
    return 0;
}
