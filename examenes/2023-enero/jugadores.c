//cuestion2
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

#define NJUGADORES 4
#define TOPEPUNTOS 5
enum estadoBalon_t {POSESION, ENAIRE};
struct jugador {
    int dorsal; // dorsal (único) del jugador
    int eficTiro; // entero entre 1 y 100 que determina su efectividad en el tiro
    int eficRebote; // entero entre 1 y 100 que determina su efectividad en el rebote
    int puntos; // puntos conseguidos durante el entrenamiento
};

int finished = 0;

#define MAXREBOTE 70
enum estadoBalon_t balon; // Modela el estado actual del balón
int dificultadRebote = 0; // var. Global para indicar la dificultad del rebote actual
struct jugador jugadores[NJUGADORES] = {
    {0, 30, MAXREBOTE, 0}, // jugador con dorsal 0, eficTiro 30, eficRebote MAXREBOTE
    {1, 80, 20, 0} ,
    {2, 40, MAXREBOTE, 0} ,
    {3, 50, 50, 0}
};


pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t rebote = PTHREAD_COND_INITIALIZER;


void correr() {
    usleep(100);
}

void tirar(struct jugador *j ) {
    pthread_mutex_lock(&m);
    
    int difTiro;
    balon = ENAIRE;
    difTiro = rand() % 100;
    //<si difTiro es menor que nuestra eficacia en tiro: canasta>
    if(difTiro<j->eficTiro){
        j->puntos+=2;
    }

    dificultadRebote = rand() % MAXREBOTE;
    if(j->puntos>TOPEPUNTOS){
        finished = 1;
    }
    //<avisar a todos los jugadores para que luchen por el rebote>
    pthread_cond_broadcast(&rebote);
    pthread_mutex_unlock(&m);

}

void rebotear(struct jugador *j )
{
    pthread_mutex_lock(&m);
    //<esperar a conseguir rebote>

    while( balon!=ENAIRE || dificultadRebote> j->eficRebote){
        pthread_cond_wait(&rebote, &m);
    }

    balon = POSESION;
    correr();

    pthread_mutex_unlock(&m);

}

void *jugadorInit(void *arg){
    /*
    pthread_mutex_lock(&m);
    struct jugador* j = (struct jugador*) arg;
    printf("caracteristicas\n");
    printf("dorsal %d\n", j->dorsal);
    printf("eficacia de tiro %d\n", j->eficTiro);
    printf("eficacia de rebote %d\n", j->eficRebote);
    printf("puntos %d\n", j->puntos);
    pthread_mutex_unlock(&m);
    */
    struct jugador* j = (struct jugador*) arg;
    while (!finished) {
        rebotear(j);
        correr();
        tirar(j);
        correr();
    }
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc!=1){
        perror("uso");
        return EXIT_FAILURE;
    }

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&rebote, NULL);
    pthread_t players[NJUGADORES];
    
    for(int i =0; i<NJUGADORES; i++){
        pthread_create(&players[i], NULL, jugadorInit, &jugadores[i]);
    }
    for(int i =0; i<NJUGADORES; i++){
        pthread_join(players[i], NULL);
    }

    for(int i = 0; i<NJUGADORES; i++){
        printf("puntuacion %d: %d\n", i, jugadores[i].puntos);
    }

    return EXIT_SUCCESS;
}
