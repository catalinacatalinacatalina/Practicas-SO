#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define NTH 10
int dispensador_posiciones = 0;
int colocados = 0;
int turno = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait = PTHREAD_COND_INITIALIZER;

void change_clothes(int id){
    printf("Runner %d is changing clothes\n", id);
	sleep((rand() % 3) + 1);
}

int get_position(int id){
    int pos;
    pthread_mutex_lock(&mutex);
    pos = dispensador_posiciones++;
    pthread_mutex_unlock(&mutex);
    return pos;
}

void move_position(int id, int pos){
    pthread_mutex_lock(&mutex);
    colocados++;
    printf("Runner %d reaches its starting position %d\n", id, pos);
    pthread_mutex_unlock(&mutex);
}

void wait_all_ready(int id){
    pthread_mutex_lock(&mutex);
    while(colocados<NTH){
        pthread_cond_wait(&wait, &mutex);
    }
    pthread_cond_broadcast(&wait);
    pthread_mutex_unlock(&mutex);
}

void wait_my_turn(int id, int pos){
    pthread_mutex_lock(&mutex);
    while(turno!=pos){
        pthread_cond_wait(&wait, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void run(int id, int pos){
    printf("Runner %d running from position %d\n", id, pos);
}

void pass_on_relay(int id){
    pthread_mutex_lock(&mutex);
    turno++;
    pthread_cond_broadcast(&wait);
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

int main(int argc, char *argv[]) {
    pthread_mutex_init(&mutex, NULL);
    pthread_t hilos [NTH];

    for(int i=0; i<NTH; i++){

        pthread_create(&hilos[i], NULL, thmain,(void*)(long) i);
    }
    for(int i=0; i<NTH; i++){
        pthread_join(hilos[i], NULL);
    }
}