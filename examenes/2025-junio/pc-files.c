#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_SBUFFER_SIZE 4
#define MAX_LINE_LENGTH 256
char* shared_buffer[MAX_SBUFFER_SIZE];
int ini = 0;
int fin = 0;
int ctr = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

typedef struct {
    FILE* e;
    FILE* s;
} es_arg_t;


void* productor(void *arg){
    es_arg_t *args = (es_arg_t*)arg;
    FILE* e = args->e;
    char line[MAX_LINE_LENGTH];

    while(fgets(line, sizeof(line), e)){
        pthread_mutex_lock(&mutex);
        while(ctr >= MAX_SBUFFER_SIZE){
            pthread_cond_wait(&not_full, &mutex);
        }

        char* l = malloc(sizeof(line)*strlen(line));
        strcpy(l, line);
        shared_buffer[fin] =l;
        fin = (fin+1) % MAX_SBUFFER_SIZE; 
        ctr++;
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }
    // termino las palabras e inserto un null
    pthread_mutex_lock(&mutex);
    while(ctr >= MAX_SBUFFER_SIZE)
        pthread_cond_wait(&not_full, &mutex);
    shared_buffer[fin] = NULL;
    fin = (fin+1) % MAX_SBUFFER_SIZE;
    ctr++;
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* consumidor(void *arg) {
    es_arg_t *args = (es_arg_t*)arg;
    FILE*s = args->s;
    while (1) {   
        pthread_mutex_lock(&mutex);
    
        while(ctr==0){
            pthread_cond_wait(&not_empty, &mutex);
        }
        //escribir elemento[i] en archivo
        char* pal = shared_buffer[ini];
        ini = (ini + 1) % MAX_SBUFFER_SIZE;
        ctr--;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        if(pal == NULL) break;

        fprintf(s, "%s", pal);
        free(pal);
    }
    return NULL;
}


int main(int argc, char *argv[]) {
    int opt;
    char* entrada=NULL;
    char* salida = NULL;
    int h = 0;
    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
            case 'i':
                entrada = optarg;
                break;
            case 'o':
                salida = optarg;
                break;
            case 'h':
                h = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-i fichero_entrada] [-o fichero_salida] [-h]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    FILE *dentrada = (entrada == NULL) ? stdin  : fopen(entrada, "r");
    FILE *dsalida  = (salida  == NULL) ? stdout : fopen(salida,  "w");
    
    if(h == 1){
        printf("Usage: [-i fichero_entrada] [-o fichero_salida] [-h]\n");
    }else{
        pthread_t prod;
        pthread_t cons;
        es_arg_t *arg = malloc(sizeof(es_arg_t));
        arg->e = dentrada;
        arg->s = dsalida;

        pthread_create(&prod, NULL, productor, arg);
        pthread_create(&cons, NULL, consumidor, arg);

        pthread_join(prod, NULL);
        pthread_join(cons, NULL);

    }
}