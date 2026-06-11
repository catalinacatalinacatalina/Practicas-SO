#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N 3
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t esperar_turno = PTHREAD_COND_INITIALIZER;
int dispensador_profes = 0;
int dispensador_alumnos = 0;
int turno_profes = 1;
int turno_alumnos = 1;
int profes_esperando=0;
int occupancy = 0;
int abierto = 1;
typedef struct {
    int id;
    int type; // 0 = student, 1 = professor
} user_arg_t;

#define TYPE(type) ((type) ? "professor" : "student")


void* comportamiento_conserje(){
    while(1){
        sleep(4);
        pthread_mutex_lock(&mutex);
        printf("Library is closing for vacation.\n");
        abierto = 0;
        pthread_mutex_unlock(&mutex);
        sleep(2);
        pthread_mutex_lock(&mutex);
        abierto = 1;
        printf("Library is now open after vacation.\n");
        pthread_cond_broadcast(&esperar_turno);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void student(int id){

    pthread_mutex_lock(&mutex);
    int turno = ++dispensador_alumnos;
    printf("User %d (student) waiting on the queue.\n", id);
    while(turno!=turno_alumnos || occupancy>= N || profes_esperando>0||abierto==0){
        pthread_cond_wait(&esperar_turno, &mutex);
    }

    occupancy++;
    turno_alumnos++;
    pthread_mutex_unlock(&mutex);
}

void professor(int id){
    pthread_mutex_lock(&mutex);
    int turno = ++dispensador_profes;
    profes_esperando++;
    printf("User %d (professor) waiting on the queue.\n", id);
    while(turno!=turno_profes || occupancy>= N || abierto==0){
        pthread_cond_wait(&esperar_turno, &mutex);
    }

    occupancy++;
    turno_profes++;
    profes_esperando--;
    pthread_mutex_unlock(&mutex);
}

void enter(int id, int type){
    int time = 2;
    printf("User %2d (%s) is reading books for %d seconds.\n", id, TYPE(type), time);
	sleep(time);
}

void exit_lib(int id, int type){
    pthread_mutex_lock(&mutex);
    printf("User %2d (%s) leaves the library.\n", id, TYPE(type));
    occupancy--;
    pthread_cond_broadcast(&esperar_turno);
    pthread_mutex_unlock(&mutex);
}

void *user(void *arg){
    user_arg_t *info = (user_arg_t *)arg;

    if(info->type)
        professor(info->id);
    else
        student(info->id);
    
    enter(info->id, info->type);
    exit_lib( info->id, info->type);
}

int main(int argc, char *argv[])
{
    FILE *fp ;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&esperar_turno, NULL);

	if((fp=fopen("ejemplo.txt", "r"))==NULL) {return 1;}
	
	int num;
	fscanf(fp, "%d", &num);
	
	pthread_t threads[num];
	pthread_t conserje;
    pthread_create(&conserje, NULL, comportamiento_conserje, NULL);

	int type;
	for (int i = 0; i < num; i++) {
		fscanf(fp, "%d", &type);
		
		user_arg_t *arg = malloc(sizeof(user_arg_t));
        arg->id = i;
		arg->type= type;

        if(pthread_create(&threads[i], NULL, user, arg)){
            perror("error al crear hilo");
            return EXIT_FAILURE;
        } 
	}
	for(int i = 0; i<num; i++){
		pthread_join(threads[i], NULL);	
	}	
	
    pthread_cancel(conserje);
	fclose(fp);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&esperar_turno);

	return EXIT_SUCCESS;
}