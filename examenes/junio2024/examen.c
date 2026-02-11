#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define N 3
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
typedef struct{
    int profe;
    int id;
}usuario_t;

int dispensador_estudiantes=0;
int turno_act_estudiantes=0;
int turno_act_profesores =0;
int dispensador_profes =0;
int aforo_actual=0;
int profes_esperando=0;
int biblioteca_abierta = 1;

void entra_estudiante(){
    pthread_mutex_lock(&mutex);
    int turno = dispensador_estudiantes++;

    while(aforo_actual == N || profes_esperando>0 || turno_act_estudiantes!= turno || !biblioteca_abierta){
        pthread_cond_wait(&cond, &mutex);
    }
    turno_act_estudiantes++;
    aforo_actual++;
    printf("ENTRADA -- Aforo %d\n", aforo_actual);

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

void entra_profesor(){
    pthread_mutex_lock(&mutex);
    int turno = dispensador_profes++;
    profes_esperando++;
    while(aforo_actual == N || turno_act_profesores!=turno || !biblioteca_abierta){
        pthread_cond_wait(&cond, &mutex);
    }
    turno_act_profesores++;
    profes_esperando--;
    aforo_actual++;
    printf("ENTRADA -- Aforo: %d\n", aforo_actual);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

void permanece(int id){
    printf("Estudiando ... %d\n", id);
    sleep(2);
}

void sal(){
    pthread_mutex_lock(&mutex);
    aforo_actual--;
    printf("SALIDA -- Aforo: %d\n", aforo_actual);

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

void * usuario(void * arg){
    usuario_t *info = (usuario_t *)arg;
    printf("Se pone a la lista: %d\n", info->id);
    if(info->profe){
        entra_profesor();
    }else{
        entra_estudiante();
    }
    permanece(info->id);
    sal();
    free(info);

    return NULL;
}
void* cerrajero(void * arg){
    while(1){
        
        pthread_mutex_lock(&mutex);
        int abierta = biblioteca_abierta;
        pthread_mutex_unlock(&mutex);

        if (abierta) {
            sleep(4); // abierta 4s
            pthread_mutex_lock(&mutex);
            printf("CERRAMOS\n");
            biblioteca_abierta = 0;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mutex);
        } else {
            sleep(2); // cerrada 2s
            pthread_mutex_lock(&mutex);
            printf("ABRIMOS\n");
            biblioteca_abierta = 1;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mutex);
        }
    }
    
}

int main(int argc, char *argv[])
{
    FILE *fp ;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	if((fp=fopen("input.txt", "r"))==NULL) {return 1;}
	
	int num;
	fscanf(fp, "%d", &num);
	printf("Numero de clientes: %d\n", num);
	
	pthread_t threads[num];
	int cliente;
    pthread_t conserje;
    if(pthread_create(&conserje, NULL ,cerrajero, NULL)){
        perror("error al crear hilo");
        return EXIT_FAILURE;
    }
	for (int i = 0; i < num; i++) {
		fscanf(fp, "%d", &cliente);
		printf("creando hilo: %d\n", i);
		usuario_t *arg = malloc(sizeof(usuario_t));
        arg->id = i;
		arg->profe= cliente;

		//crear hilo
		if(pthread_create(&threads[i], NULL ,usuario, arg)){
			perror("error al crear hilo");
			return EXIT_FAILURE;
		}
	}

	for(int i = 0; i<num; i++){
		pthread_join(threads[i], NULL);	
	}
    // Cuando todos los usuarios hayan salido, cancelar y unir el cerrajero
    pthread_cancel(conserje);
    pthread_join(conserje, NULL);
	fclose(fp);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return EXIT_SUCCESS;
}
