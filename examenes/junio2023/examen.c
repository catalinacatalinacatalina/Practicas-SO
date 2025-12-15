#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define NTH 10
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t esperar_demas = PTHREAD_COND_INITIALIZER;
pthread_cond_t esperar_turno = PTHREAD_COND_INITIALIZER;

typedef struct {
    int dorsal;
    int posicion;
} corredor_t;

int dispensador_posiciones =0;
int turno =0;
int waiting =0;

void change_clothes(int id){
    printf("Runner %d changing clothes\n", id);
    sleep(rand()%3+1);
}
int get_position(int id){
    pthread_mutex_lock(&mutex);
    int pos= dispensador_posiciones++;
    pthread_mutex_unlock(&mutex);
    return pos;
}
void move_position(int id, int pos){
    pthread_mutex_lock(&mutex);
    waiting++;
    // Si este es el último en colocarse, despierta a los demás
    if (waiting == NTH) {
        pthread_cond_broadcast(&esperar_demas);
    }
    pthread_mutex_unlock(&mutex);
    printf("Runner %d reaches its starting position %d\n", id, pos);
}

void wait_all_ready(int id){
    pthread_mutex_lock(&mutex);
    while (waiting<NTH) {
        pthread_cond_wait(&esperar_demas, &mutex);
    }
    pthread_cond_broadcast(&esperar_demas);
    pthread_mutex_unlock(&mutex);
}

void wait_my_turn(int id, int pos){
    pthread_mutex_lock(&mutex);
    while (turno != pos) {
        pthread_cond_wait(&esperar_turno, &mutex);
    }

    pthread_mutex_unlock(&mutex);
}

void run(int id, int pos){
    sleep(rand()%3+1);
    printf("Runner %d running from position %d\n", id, pos);
}

void pass_on_relay(int id){
    pthread_mutex_lock(&mutex);
    // Pasa el turno al siguiente corredor
    turno++;
    // Despierta a los hilos que están esperando su turno
    pthread_cond_broadcast(&esperar_turno);
    pthread_mutex_unlock(&mutex);
}

void *thmain(void *arg)
{
    int id = (int) (long long) arg;
    int pos;
    change_clothes(id);
    pos = get_position(id);
    move_position(id, pos);
    wait_all_ready(id);
    wait_my_turn(id, pos);
    run(id, pos);
    pass_on_relay(id);
    return NULL;
}


int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutex, NULL);

	pthread_t threads[NTH];
	int dorsal;
	for (int i = 0; i < NTH; i++) {
		
		dorsal = i;

		//crear hilo
		if(pthread_create(&threads[i], NULL, thmain,(void*)(long long) dorsal)){
			perror("error al crear hilo");
			return EXIT_FAILURE;
		}

	}

	for(int i = 0; i<NTH; i++){
		pthread_join(threads[i], NULL);	
	}	
	pthread_mutex_destroy(&mutex);


	return EXIT_SUCCESS;
}
