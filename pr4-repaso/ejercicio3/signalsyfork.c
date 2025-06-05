#define _POSIX_C_SOURCE 200809L //WSL
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/*programa que temporiza la ejecución de un proceso hijo */
pid_t child_pid =-1; // Crear un proceso hijo

void SIGALRM_handler(int signum) {
	if(child_pid> 0) {
		kill(child_pid, SIGKILL); // Terminar el proceso hijo si se recibe SIGALRM
		printf("Proceso hijo terminado por SIGALRM\n");
		exit(EXIT_SUCCESS);
	}
}


int main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "Uso: %s <comando> [argumentos...]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Preparamos los argumentos del hijo (execvp necesita NULL al final)
    char **args = &argv[1];

	// configuración de la alarma
    struct sigaction sa;
    sa.sa_handler = SIGALRM_handler; // Asignar el manejador de señal
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if(sigaction(SIGALRM, &sa, NULL)== -1) {
		perror("sigaction failed");
		exit(EXIT_FAILURE);
	}

	// forkeamos el proceso hijo
	if ((child_pid = fork()) == -1) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	} 
	
	if (child_pid == 0) { // Si estamos en el hijo
		if(execvp(argv[1], &argv[1]) == -1) { // Ejecutar el comando utilizando execvp
			perror("execvp fallido");
			exit(EXIT_FAILURE);
		}
	} else { //padre
		int status;
		pid_t wpid = waitpid(child_pid, &status, 0); // Esperar al hijo
		if (wpid == -1) {
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}
		if (WIFEXITED(status)) {
            printf("[Padre] El hijo terminó normalmente con código %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Padre] El hijo terminó por la señal %d (%s)\n",
                   WTERMSIG(status), strsignal(WTERMSIG(status)));
        } else {
            printf("[Padre] El hijo terminó por otra causa.\n");
        }
	}
	return 0;
}
