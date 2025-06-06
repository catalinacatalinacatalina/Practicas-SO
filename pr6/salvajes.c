#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <asm-generic/fcntl.h>
#include <sys/mman.h>

#define NUMITER 3
#define SHM_NAME "/caldero_shm"
#define SEM_MUTEX "/sem_mutex"
#define SEM_COCINAR "/sem_cocinar"
#define SEM_LLENO "/sem_lleno"
#define NUMITER 3

int* raciones;
int getServingsFromPot(void) {
	
}

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void)
{

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


    printf("Salvaje: raciones en el caldero = %d\n", *raciones);

    // Aquí iría la lógica de restar raciones y sincronización

	sem_t *mutex = sem_open(SEM_MUTEX, 0);
    sem_t *sem_cocinar = sem_open(SEM_COCINAR, 0);
    sem_t *sem_lleno = sem_open(SEM_LLENO, 0);

    pid_t child = getpid();

	for(int i=0; i<NUMITER; i++) {
		//
		getServingsFromPot();
		eat();
		//
	}

    // Limpieza
    munmap(raciones, sizeof(int));
    close(shm_fd);
	
	return 0;
}
