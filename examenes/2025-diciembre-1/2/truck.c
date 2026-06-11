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

#define SHM_NAME "/sem_loading_docks"
#define NTHREADS 2

sem_t* sem;
pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
int current_load = 0;

typedef struct {
    int id;
    int packages;
} load_arg_t;

void* load(void* p){
    load_arg_t* arg = (load_arg_t*)p;
    pthread_mutex_lock(&mutex);
    int not_loaded = arg->packages>current_load;
    pthread_mutex_unlock(&mutex);
    while(not_loaded){
        pthread_mutex_lock(&mutex);
        if(current_load<arg->packages){
            current_load++;
            not_loaded = current_load<arg->packages;
            pthread_mutex_unlock(&mutex);
            usleep(1000);
        } else{
            not_loaded=0;
            pthread_mutex_unlock(&mutex);
        }

    }

    return NULL;
}

int main(int argc, char *argv[]){
    if(argc!=3){
        perror("uso");
        return EXIT_FAILURE;
    }
    int id = atoi(argv[1]);
    int packages = atoi(argv[2]);
    sem = sem_open(SHM_NAME, 0);

    pthread_mutex_init(&mutex, NULL);

    printf("[Truck %d] Waiting for a loading dock...\n", id);
    fflush(stdout);
    sem_wait(sem);


    pthread_t t[NTHREADS];
    load_arg_t *arg = malloc(sizeof(load_arg_t));
    arg->id = id;
    arg->packages = packages;
    printf("[Truck %d] Entering the loading dock.\n", id);
    for (int i=0; i<NTHREADS; i++) {
        pthread_create(&t[i], NULL, load, (load_arg_t*) arg);
    }

    for (int i=0; i<NTHREADS; i++) {
        pthread_join(t[i], NULL);
    }

    free(arg);
    usleep(1000*packages);
    printf("[Truck %d] Loading finished (Total loaded: %d). Exiting\n", id, current_load);
    sem_post(sem);
    sem_close(sem);
    return EXIT_SUCCESS;
}