#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 10
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

int turno_actual_vip = 1;
int turno_actual = 1;
int dispensador_turnos = 0;
int dispensador_turnos_vip = 0;
int current_ocupancy = 0;
int vip_waiting = 0;
int clean_pending =0;
int clients_since_clean =0 ; // numero clientes que han estado desde que se ha limpiado
int active_clients;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t esperar_turno = PTHREAD_COND_INITIALIZER;
pthread_t clean;

typedef struct {
    int id;
    int isvip;
} client_arg_t;


void enter_vip_client(int id)
{
	pthread_mutex_lock(&mutex);
    int turno = ++dispensador_turnos_vip;
    vip_waiting++;
    
    while (current_ocupancy >= CAPACITY || turno != turno_actual_vip || 
		(clean_pending==1 &&clients_since_clean>=3))
    {
        pthread_cond_wait(&esperar_turno, &mutex);
    } 

    turno_actual_vip++;
    vip_waiting--;
    current_ocupancy++;
	clients_since_clean++;

    //printf("Ha entrado el cliente vip: %d. Hay una capacidad de: %d\n", id, current_ocupancy);
    pthread_cond_broadcast(&esperar_turno);
    pthread_mutex_unlock(&mutex);
}

void enter_normal_client(int id)
{
	pthread_mutex_lock(&mutex);
	int turno = ++dispensador_turnos;
	
	while (current_ocupancy >= CAPACITY || turno != turno_actual || vip_waiting > 0
		|| (clean_pending==1 && clients_since_clean>=3)) {
        pthread_cond_wait(&esperar_turno, &mutex);
    }
	turno_actual++;
	//printf("CLIENTE NORMAL DISCOTECA %2d ENTRA\n", id);
	current_ocupancy++;
	clients_since_clean++;
	pthread_cond_broadcast(&esperar_turno);
	pthread_mutex_unlock(&mutex);
}


void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(&mutex);
	current_ocupancy--;
	printf("Client %2d (%s) exit. Capacity: %d\n", id, VIPSTR(isvip), current_ocupancy);
	pthread_cond_broadcast(&esperar_turno);
	pthread_mutex_unlock(&mutex);
}

void *client(void *arg)
{
	client_arg_t *info = (client_arg_t *)arg;
	if (info->isvip == 1)
	{
        enter_vip_client(info->id);
	}
    else
	{
        enter_normal_client(info->id);
	}
    dance(info->id, info->isvip);
	disco_exit(info->id, info->isvip);
	pthread_mutex_lock(&mutex);
	active_clients--;
	pthread_cond_broadcast(&esperar_turno);
	pthread_mutex_unlock(&mutex);
    free(info);
}
void enter_cleaning(){
	pthread_mutex_lock(&mutex);
	clean_pending = 1;
	while((clients_since_clean<3|| current_ocupancy>0 )&& active_clients>0){
		pthread_cond_wait(&esperar_turno, &mutex);
	}
	pthread_mutex_unlock(&mutex);
}

void exit_cleaning(){
	pthread_mutex_lock(&mutex);

	clean_pending = 0;
	clients_since_clean=0;
	pthread_cond_broadcast(&esperar_turno);
	pthread_mutex_unlock(&mutex);

}
void* cleaning_thread(void* arg){
    while (1) {

        pthread_mutex_lock(&mutex);
        if (active_clients == 0) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);

        enter_cleaning();

        pthread_mutex_lock(&mutex);
        if (active_clients == 0 || clients_since_clean < 3) {
            clean_pending = 0;
            pthread_cond_broadcast(&esperar_turno);
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);

        printf("Cleaning...\n");
        sleep(2);
        printf("Done!\n");

        exit_cleaning();
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    FILE *fp ;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&esperar_turno, NULL);

	if((fp=fopen("ejemplo.txt", "r"))==NULL) {return 1;}
	
	int num;
	fscanf(fp, "%d", &num);
	active_clients = num;
	printf("Numero de clientes: %d\n", num);
	
	pthread_t threads[num];
	
	if(pthread_create(&clean, NULL, cleaning_thread, NULL)){
		perror("error al crear hilo");
		return EXIT_FAILURE;
	}
	

	int cliente;
	for (int i = 0; i < num; i++) {
		fscanf(fp, "%d", &cliente);
		
		client_arg_t *arg = malloc(sizeof(client_arg_t));
        arg->id = i;
		arg->isvip= cliente;

		//crear hilo
		
		if(pthread_create(&threads[i], NULL ,client,arg)){
			perror("error al crear hilo");
			return EXIT_FAILURE;
		}

	}
	for(int i = 0; i<num; i++){
		pthread_join(threads[i], NULL);	
	}	
	
	pthread_join(clean, NULL);
	fclose(fp);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&esperar_turno);

	return EXIT_SUCCESS;
}
