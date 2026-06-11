#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>


int setargs(char* args, char** argv){
    int n_subcadenas = 0;
    char *start = args;
    
    // Ignorar espacios al comienzo
    while (*start && isspace(*start)) {
        start++;
    }
    
    while(*start){
        // PASO 1: Guardar dirección del inicio de la palabra
        if (argv != NULL) {
            argv[n_subcadenas] = start;
        }
        n_subcadenas++;
        
        // PASO 2: Avanzar mientras sea parte de la palabra (no espacio)
        while (*start && !isspace(*start)) {
            start++;
        }

        // PASO 3: Si hay espacio -> reemplazarlo por '\0'
        if(*start && isspace(*start)){
            if(argv != NULL){
                *start = '\0';  // Reemplazar espacio por '\0'
            }
            start++;  // Avanzar al siguiente carácter
        }
        
        // PASO 4: Saltar espacios múltiples
        while (*start && isspace(*start)) {
            start++;
        }
    }
    
    // Terminar array con NULL si argv != NULL
    if (argv != NULL) {
        argv[n_subcadenas] = NULL;
    }
    
    return n_subcadenas;
}

int main(int argc, char *argv[]) {
    if (argc!=2){
        perror("uso");
        return EXIT_FAILURE;
    }

    char*  filepath = argv[1];
    FILE* fp = fopen(filepath, "r");

    if(!fp){
        perror("archivo");
        return EXIT_FAILURE;
    }
    char line[1024];

    while (fgets(line, sizeof(line), fp)) {
        int ncadenas;
        ncadenas = setargs(line, NULL);
        printf("ncadenas %d\n", ncadenas);
        char** cargv = malloc(sizeof(char*)* (ncadenas+1));
        setargs(line, cargv);

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if(pid == 0){
            execvp(cargv[0], cargv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }else{
            int status;
            waitpid(pid, &status, 0);
        }

    }

    
    return EXIT_SUCCESS;
}