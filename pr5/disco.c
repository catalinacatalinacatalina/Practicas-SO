#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")
#define MAXAFORO 10

int turno_actual_vip = 1;
int turno_actual = 1;
int dispensador_turnos = 0;
int dispensador_turnos_vip = 0;
int current_ocupancy = 0;
int vip_waiting = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t esperar_turno = PTHREAD_COND_INITIALIZER;

typedef struct {
    int id;
    int isvip;
} client_arg_t;


void enter_vip_client(int id)
{
	pthread_mutex_lock(&mutex);
    int turno = ++dispensador_turnos_vip;
    vip_waiting++;
    
    while (current_ocupancy >= MAXAFORO || turno != turno_actual_vip )
    {
        pthread_cond_wait(&esperar_turno, &mutex);
    } 

    turno_actual_vip++;
    vip_waiting--;
    current_ocupancy++;

    printf("Ha entrado el cliente vip: %d. Hay una capacidad de: %d\n", id, current_ocupancy);
    pthread_cond_broadcast(&esperar_turno);
    pthread_mutex_unlock(&mutex);
}

void enter_normal_client(int id)
{
	pthread_mutex_lock(&mutex);
	int turno = ++dispensador_turnos;
	
	while (current_ocupancy >= MAXAFORO || turno != turno_actual || vip_waiting > 0) {
        pthread_cond_wait(&esperar_turno, &mutex);
    }
	turno_actual++;
	printf("CLIENTE NORMAL DISCOTECA %2d ENTRA\n", id);
	current_ocupancy++;
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
    free(info);
}

int main(int argc, char *argv[])
{
    FILE *fp ;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&esperar_turno, NULL);

	if((fp=fopen("ejemplo.txt", "r"))==NULL) {return 1;}
	
	int num;
	fscanf(fp, "%d", &num);
	printf("Numero de clientes: %d\n", num);
	
	pthread_t threads[num];
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
	fclose(fp);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&esperar_turno);

	return EXIT_SUCCESS;
}
