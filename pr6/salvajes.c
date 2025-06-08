#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

#define NUMITER 3
#define SHM_NAME "/caldero_shm"
#define SEM_MUTEX "/sem_mutex"
#define SEM_COCINAR "/sem_cocinar"
#define SEM_LLENO "/sem_lleno"

int* raciones;
sem_t *mutex, *sem_cocinar, *sem_lleno;
void getServingsFromPot(int id) {
	sem_wait(mutex);
    if (*raciones == 0) {
        printf("[SALVAJE %d] El caldero está vacío. Avisando al cocinero...\n", id);
        sem_post(sem_cocinar);
        sem_wait(sem_lleno);
    }
    (*raciones)--;
    printf("[SALVAJE %d] Ha tomado una ración. Raciones restantes: %d\n", id, *raciones);
    sem_post(mutex);
}


void eat(int id)
{

	printf("[SALVAJE %d] Comiendo...\n", id);
	sleep(rand() % 5);
}



int main(int argc, char *argv[])
{


	int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        fprintf(stderr, "Ejecuta primero el cocinero\n");
        exit(EXIT_FAILURE);
    }

    raciones = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (raciones == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    mutex = sem_open(SEM_MUTEX, 0);
    sem_cocinar = sem_open(SEM_COCINAR, 0);
    sem_lleno = sem_open(SEM_LLENO, 0);

	if (mutex == SEM_FAILED || sem_cocinar == SEM_FAILED || sem_lleno == SEM_FAILED) {
		perror("sem_open");
		exit(EXIT_FAILURE);
	}

	int id = getpid();
	for(int i=0; i<NUMITER; i++) {
		getServingsFromPot(id);
		eat(id);
	}
    
	
	printf("[SALVAJE %d] Ha terminado.\n", id);


	return 0;
}
