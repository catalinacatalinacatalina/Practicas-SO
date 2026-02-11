#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <errno.h>
#include <pthread.h>
#define NJUGADORES 4
#define MAXREBOTE 70
#define TOPEANOTACION 5

struct jugador {
    int dorsal; // dorsal (único) del jugador
    int eficTiro; // entero entre 1 y 100 que determina su efectividad en el tiro
    int eficRebote; // entero entre 1 y 100 que determina su efectividad en el rebote
    int puntos; // puntos conseguidos durante el entrenamiento
};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t esperar = PTHREAD_COND_INITIALIZER;
enum estadoBalon_t {POSESION, ENAIRE};

enum estadoBalon_t balon; // Modela el estado actual del balón

int dificultadRebote =0; // var. Global para indicar la dificultad del rebote actual
int finished = 0;
struct jugador jugadores[NJUGADORES] = {
    {0,30,MAXREBOTE,0}, // jugador con dorsal 0, eficTiro 30, eficRebote MAXREBOTE
    {1,80,20,0} ,
    {2,40,MAXREBOTE,0} ,
    {3,50,50,0}
};

void tirar(struct jugador* j){
    int difTiro;
    pthread_mutex_lock(&mutex);
    balon = POSESION;
    difTiro = rand() % 100;
    //printf("difTiro: %d\n", difTiro);
    //<si difTiro es menor que nuestra eficacia en tiro: canasta>
    if(difTiro<=j->eficTiro){
        printf("ha metido canasta el dorsal %d \n",j->dorsal);
        dificultadRebote = rand() % MAXREBOTE;
        j->puntos+=2;
        //<si hemos sobrepasado TOPEPUNTOS, termina el entrenamiento> -- DONE
        if(j->puntos>=TOPEANOTACION) {
            finished=1;
            printf("finalizado - GANA: %d\n", j->dorsal);
        }
        //<avisar a todos los jugadores para que luchen por el rebote>
        pthread_cond_broadcast(&esperar);
    }
    pthread_mutex_unlock(&mutex);
}

void rebotar(struct jugador* j){
    pthread_mutex_lock(&mutex);
    //<esperar a conseguir rebote>
    printf("Esperando rebote de %d\n", j->dorsal);

    while(balon!=POSESION){
        pthread_cond_wait(&esperar, &mutex);
    }

    printf("Rebote de %d\n", j->dorsal);
    balon = POSESION; 
    pthread_cond_broadcast(&esperar); // avisar a otros hilos
    pthread_mutex_unlock(&mutex);
}

void correr(){
    sleep(1);
}

void* jugadorInit(void* arg){
    struct jugador *j = (struct jugador *)arg;

    printf("jugador %i (tiro: %d, rebote: %d)\n", j->dorsal, j->eficTiro, j->eficRebote);
    
    while(!finished){
        rebotar(j);
        correr();
        tirar(j);
        correr();
    }

    printf("jugador %i -- puntos: %i\n", j->dorsal, j->puntos);
    
    return NULL;
}

int main(){
    pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&esperar, NULL);
    pthread_t threads[NJUGADORES];

    for(int i =0; i<NJUGADORES;i++){
		if(pthread_create(&threads[i], NULL ,jugadorInit, &jugadores[i])){
			perror("error al crear hilo");
			return EXIT_FAILURE;
		}
    }

    for(int i =0; i<NJUGADORES; i++)
        pthread_join(threads[i], NULL);
    return 0;
}