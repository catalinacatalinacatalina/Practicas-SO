#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")
#define MAXAFORO 10

pthread_mutex_t mi_mutex;
pthread_cond_t cond_vip;
pthread_cond_t cond_normal;
int aforo = 0;
int max_aforo = MAXAFORO;
int esperando_vip = 0;
int esperando_normal = 0;

typedef struct {
    int id;
    int isvip;
} client_arg_t;

void enter_normal_client(int id)
{
	printf("CLIENTE NORMAL DISCOTECA %2d ENTRA\n", id);
	aforo++;
}

void enter_vip_client(int id)
{
	printf("CLIENTE VIP DISCOTECA %2d ENTRA\n", id);
	aforo++;
}

void dance(int id, int isvip)
{
	printf("CLIENTE %2d (%s) BAILANDO\n", id, VIPSTR(isvip));
	usleep(1000000);
	printf("CLIENTE %2d (%s) TERMINA DE BAILAR\n", id, VIPSTR(isvip));
}

void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(&mi_mutex);
	aforo--;
	printf("Client %2d (%s) exit disco\n", id, VIPSTR(isvip));
	if (esperando_vip > 0)
		pthread_cond_broadcast(&cond_vip);
	else
		pthread_cond_broadcast(&cond_normal);
	pthread_mutex_unlock(&mi_mutex);
}

void *client(void *arg)
{
    client_arg_t *info = (client_arg_t *)arg;
	//pthread_mutex_lock(&mi_mutex);

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
    //pthread_mutex_unlock(&mi_mutex);

    free(info);
}

int main(int argc, char *argv[])
{
    FILE *fp ;
	pthread_mutex_init(&mi_mutex, NULL);
	pthread_cond_init(&cond_vip, NULL);
	pthread_cond_init(&cond_normal, NULL);

	if((fp=fopen("ejemplo.txt", "r"))==NULL) {return 1;}
	
	int num;
	fscanf(fp, "%d", &num);


	printf("Number of clients: %d\n", num);
	pthread_t threads[num];
	for	(int i = 0; i < num; i++) {
		char vip_char;
        fscanf(fp, " %c", &vip_char);

        client_arg_t *arg = malloc(sizeof(client_arg_t));
        arg->id = i;
        arg->isvip = (vip_char == '1') ? 1 : 0;
        if(pthread_create(&threads[i], NULL, client, arg)!= 0) {
			perror("Error creating thread");
			free(arg);
			fclose(fp);
			pthread_mutex_destroy(&mi_mutex);
			pthread_cond_destroy(&cond_vip);
			pthread_cond_destroy(&cond_normal);
			exit(EXIT_FAILURE);
		}
	}

	for	(int i = 0; i < num; i++) {
		pthread_join(threads[i], NULL);
	}

	fclose(fp);
	pthread_mutex_destroy(&mi_mutex);
	pthread_cond_destroy(&cond_vip);
    pthread_cond_destroy(&cond_normal);
	return 0;
}
