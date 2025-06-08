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
int getServingsFromPot(void) {
	int cogido = 0;
	int avisado = 0;
	while (!cogido) {
		sem_wait(mutex); // entrar en sección crítica

		if (*raciones == 0 && !avisado) {
			printf("[SALVAJE %d] Caldero vacío, aviso al cocinero\n", getpid());
			sem_post(sem_cocinar);  // avisar al cocinero que rellene
			sem_wait(sem_lleno);    // esperar a que haya raciones
			avisado = 1; // marcar que ya se ha avisado al cocinero
		}

		if (*raciones > 0) {
			(*raciones)--;
			printf("[SALVAJE %d] Coge ración, quedan %d\n", getpid(), *raciones);
			sem_post(mutex); // salir de sección crítica
			avisado = 0; // resetear aviso para el siguiente ciclo
			cogido = 1; // ya he cogido mi ración
		} else {
			sem_post(mutex); // salir si no había raciones, para volver a intentar
			// pequeño retardo para evitar busy wait muy agresivo
			usleep(1000);
		}
	}
	return 0;
}


void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void savages(void)
{
	for(int i=0; i<NUMITER; i++) {
		getServingsFromPot();
		eat();
	}
}

int main(int argc, char *argv[])
{
	srand(time(NULL) ^ (getpid()<<16));
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

    savages();

    // Limpieza
    munmap(raciones, sizeof(int));
    close(shm_fd);

	return 0;
}
