#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Definimos una estructura que contendrá el número de hilo y la prioridad
typedef struct {
    int numero_hilo;
    char prioridad;
} hilo_arg_t;

// Función que será ejecutada por cada hilo
void *thread_usuario(void *arg) {
    // Convertimos el argumento a un puntero a la estructura hilo_arg_t
    hilo_arg_t *hilo_arg = (hilo_arg_t *)arg;

    // Obtenemos el ID del hilo
    pthread_t hilo_id = pthread_self();

    // Imprimimos la información del hilo
    printf("Hilo %ld: Número de hilo = %d, Prioridad = %c\n", hilo_id, hilo_arg->numero_hilo, hilo_arg->prioridad);

    // Liberamos la memoria dinámica para el argumento
    free(hilo_arg);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Por favor, ingresa el número de hilos a crear.\n");
        return 1;
    }

    int num_hilos = atoi(argv[1]);
    pthread_t hilos[num_hilos];

    // Creamos los hilos
    for (int i = 0; i < num_hilos; i++) {
        // Reservamos memoria dinámica para el argumento de cada hilo
        hilo_arg_t *arg = (hilo_arg_t *)malloc(sizeof(hilo_arg_t));
        if (arg == NULL) {
            perror("Error al reservar memoria");
            return 1;
        }

        // Inicializamos los campos de la estructura
        arg->numero_hilo = i + 1;
        // Los hilos impares no son prioritarios, los pares sí
        arg->prioridad = (arg->numero_hilo % 2 == 0) ? 'P' : 'N';

        // Creamos el hilo y pasamos el argumento
        if (pthread_create(&hilos[i], NULL, thread_usuario, (void *)arg) != 0) {
            perror("Error al crear el hilo");
            return 1;
        }
    }

    // Esperamos a que todos los hilos finalicen
    for (int i = 0; i < num_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }

    return 0;
}

