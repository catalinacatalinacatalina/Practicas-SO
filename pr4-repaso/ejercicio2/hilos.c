#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct {
    int numero_hilo;
    char prioridad;
} thread_args_t;

void *thread_usuario(void *arg)
{

}


int main(int argc, char* argv[])
{
	
	pthread_t th1;
	if(pthread_create(&th1, NULL, thread_usuario, NULL) != 0) {
		perror("Error creating thread");
		exit(EXIT_FAILURE);
	}

	pthread_join(th1, NULL);

	return 0;
}
