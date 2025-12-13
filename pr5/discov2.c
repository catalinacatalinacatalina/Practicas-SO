#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CAPACITY 10
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

int turno_actual_vip = 1;
int turno_actual = 1;
int dispensador_turnos = 0;
int dispensador_turnos_vip = 0;
int current_ocupancy = 0;
int vip_waiting = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t esperar_turno = PTHREAD_COND_INITIALIZER;

void enter_vip_client(int id)
{
    pthread_mutex_lock(&mutex);
    int turno = ++dispensador_turnos_vip;
    vip_waiting++;
    
    while (current_ocupancy >= CAPACITY || turno != turno_actual_vip )
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

    while (current_ocupancy >= CAPACITY || turno != turno_actual || vip_waiting > 0) {
        pthread_cond_wait(&esperar_turno, &mutex);
    }
    turno_actual++;
    current_ocupancy++;

	printf("\n");
    printf("Ha entrado el cliente normal: %d. Hay una capacidad de: %d\n", id, current_ocupancy);
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
	int id = ((int*)arg)[0];
	int esvip = ((int*)arg)[1];

	if(esvip)
		enter_vip_client(id);
	else
		enter_normal_client(id);
	dance(id, esvip);

	disco_exit(id, esvip);
}

int main(int argc, char *argv[])
{
	FILE *fp ;

	if(!(fp = fopen(argv[1], "r"))){
		return EXIT_FAILURE;
	}
	int num;
	if(fscanf(fp, "%d", &num)!=1){
		fclose(fp);
		return EXIT_FAILURE;
	}
	pthread_t threads[num];

	for (int i = 0; i < num; i++) {
		int *args = malloc(2*sizeof(int));
		if(!args){
			fclose(fp);
			return EXIT_FAILURE;
		}

		args[0]=i+1;
		if(fscanf(fp, "%d", &args[1])!=1){
			free(args);
			fclose(fp);
			return EXIT_FAILURE;
		}

		if(pthread_create(&threads[i], NULL, client, args)!=0){
            perror("error al crear hilo");
			free(args);
			fclose(fp);
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