#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#define MAXLEN_LINE_FILE 100

int setArgs(char* args, char **argv){
    int ret = 0;
    char* it = args;
    char* begin_str = args;
    // Quitar espacios al principio
    while(isspace(*it++));

    while(!(*it == '\0')) {
        if(isspace(*it)){
            if(argv!= NULL){
                *it = '\0';
                it++;
                argv[ret++] = begin_str;
                begin_str = it;
                printf("Argv[%d] = '%s'\n", ret, argv[ret]);
            }else{
                ret++;
                it++;
            }
        } else{
            it++;
        }
    }
     if (argv != NULL) {
        argv[ret] = begin_str;
    }
    return ret + 1;
}

int main(int argc, char *argv[]){
    if(argc != 2) {
        printf("El numero de argumentos no es el adecuado, %d\n", argc);
        return EXIT_FAILURE;
    }
    printf("File to read: %s\n", argv[1]);
    FILE *fd = fopen(argv[1], "r");
    if(fd == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

	char line[256];
    printf("Reading file: %s\n", argv[1]);
    while(fgets(line, sizeof(line), fd) != NULL) {
        printf("Line read: %s \n", line);
        int len = strlen(line);

        if (line[len - 1] == '\n')
            line[len - 1] = '\0';

        printf("Line after removing newline: %s\n", line);

        int ntokens = setArgs(line, NULL);
        printf("Number of args: %d\n", ntokens);


        char **cargv = malloc(sizeof(char*) * (ntokens));
        cargv[ntokens] = NULL; // Set last element to NULL for execvp
        
        setArgs(line, cargv);

        printf("Ejecutando comando: ");
        for (int i = 0; i < ntokens; i++) {
            printf("%s ", cargv[i]);
        }
        printf("\n------------------------------\n");

        pid_t pid = fork();
        if(pid < 0) {
            perror("Fork failed");
            free(cargv);
            fclose(fd);
            return EXIT_FAILURE;
        } else if(pid == 0) {
            // Child process
            execvp(cargv[0], cargv);
            perror("Exec failed");
            exit(EXIT_FAILURE);
        }
        free(cargv);
    }


    fclose(fd);
    return 0;
}

