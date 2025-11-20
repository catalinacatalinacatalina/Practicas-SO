
#define _POSIX_C_SOURCE 200809L // SOLO EN WSL
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t child_pid; // PID del proceso hijo

// Manejador para la señal SIGALRM
void manejador_alarma(int signo) {
    if (child_pid > 0) {
        printf("Señal SIGALRM recibida. Terminando el proceso hijo (PID: %d)...\n", child_pid);
        kill(child_pid, SIGKILL); // Envía SIGKILL al proceso hijo
    }
}

// Manejador para ignorar SIGINT
void ignorar_sigint(int signo) {
    printf("SIGINT ignorado por el proceso padre.\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando> [argumentos...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Configuramos el manejador para SIGALRM
    struct sigaction sa;
    sa.sa_handler = manejador_alarma;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("Error al configurar sigaction para SIGALRM");
        exit(EXIT_FAILURE);
    }

    // Configuramos el manejador para ignorar SIGINT
    struct sigaction sa_ignore;
    sa_ignore.sa_handler = ignorar_sigint;
    sa_ignore.sa_flags = 0;
    sigemptyset(&sa_ignore.sa_mask);

    if (sigaction(SIGINT, &sa_ignore, NULL) == -1) {
        perror("Error al configurar sigaction para SIGINT");
        exit(EXIT_FAILURE);
    }

    // Creamos un proceso hijo
    child_pid = fork();
    if (child_pid == -1) {
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Código del proceso hijo
        printf("Proceso hijo creado. Ejecutando el comando: %s\n", argv[1]);
        execvp(argv[1], &argv[1]); // Cambiamos el ejecutable del hijo
        perror("Error en execvp"); // Si execvp falla
        exit(EXIT_FAILURE);
    } else {
        // Código del proceso padre
        printf("Proceso padre: PID del hijo es %d\n", child_pid);

        // Programamos la alarma para 5 segundos
        alarm(5);

        // Esperamos al hijo
        int status;
        pid_t terminated_pid = wait(&status);

        if (terminated_pid == -1) {
            perror("Error al esperar al proceso hijo");
            exit(EXIT_FAILURE);
        }

        // Verificamos cómo terminó el hijo
        if (WIFEXITED(status)) {
            printf("El proceso hijo terminó normalmente con código de salida %d.\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("El proceso hijo terminó por señal: %s.\n", strsignal(WTERMSIG(status)));
        }
    }

    return 0;
}

