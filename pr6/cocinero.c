#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <bits/semaphore.h>

#define M 10
#define SHM_NAME "/caldero_shm"
#define SEM_MUTEX "/sem_mutex"
#define SEM_COCINAR "/sem_cocinar"
#define SEM_LLENO "/sem_lleno"


int finish = 0;
int *raciones;
int shm_fd;
sem_t *mutex, *sem_cocinar, *sem_lleno;

void putServingsInPot(int servings)
{
}

void cook(void)
{
	while(!finish) {
		putServingsInPot(M);
	}
}

void handler(int signo)
{
	finish = 1;
}

int main(int argc, char *argv[])
{
	int shm_fd;

	if((shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)) < 0) {
		perror("shm_open");
		exit(EXIT_FAILURE);
	}

	// Ajusta el tamaño del segmento de memoria
    if (ftruncate(shm_fd, sizeof(int)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }


 	// Mapear memoria en el espacio del proceso
    int* raciones = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (raciones == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

	*raciones = M;
	printf("Caldero creado con %d raciones\n", *raciones);

	getchar(); // Espera a que el usuario presione una tecla para continuar

	//limpieza
	if (munmap(raciones, sizeof(int)) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}
	close(shm_fd);
	shm_unlink(SHM_NAME);

	return 0;
}
