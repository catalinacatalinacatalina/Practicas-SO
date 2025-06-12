
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
int setArgs(char *args, char **argv)
{
    char *aux = args; // Pointer to the start of the current substring

    // cuerpo de la funcion
    while (isspace(*aux))
    {
        aux++;
    }

    int count = 0; // Count of characters in the current substring
    char *begin_str = aux;
    while (*aux != '\0')
    {
        //printf("Current char: '%c'\n", *aux);
        if (isspace(*aux))
        {
            if (argv != NULL){
                *aux = '\0'; // Replace space with null terminator
                argv[count] = begin_str; // Store the beginning of the substring
            }
            count++; // Count the number of arguments

            begin_str = aux + 1;
        }
        aux++;
    }
    // ultimo argumento
    if(argv != NULL){
        *aux = '\0'; // Replace space with null terminator
        argv[count] = begin_str;
    }

    count++;

    //printf("Count: %d\n", count);
    //printf("Last argument: '%s'\n", begin_str);

    if (argv == NULL)
    {
        return count; // If argv is NULL, we cannot store any arguments
    }

    for (int i = 0; i < count; i++)
    {
        printf("Argv[%d] = '%s'\n", i, argv[i]);
    }

    return count;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("El numero de argumentos no es el adecuado, %d\n", argc);
        return EXIT_FAILURE;
    }

    FILE *fd = fopen(argv[1], "r");
    if (fd == NULL)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    char line[256];

    while(fgets(line, sizeof(line), fd) != NULL)
    {
        int subcadenas = setArgs(line, NULL);
        int len = strlen(line);
        if (line[len - 1] == '\n')
            line[len - 1] = '\0';

        char **cargv = malloc(sizeof(char*) * (len+1));
        cargv[len] = NULL; // Set last element to NULL for execvp
        cargv[subcadenas] = NULL; // Set last element to NULL for execvp
        
        int val2 = setArgs(line, cargv);

        pid_t pid = fork();
        if(pid==0){
            // cargv = echo "hola"
            execvp(cargv[0], cargv);
            perror("Error executing command");
            exit(EXIT_FAILURE);
        }
        while(wait(NULL) == -1); 


        free(cargv);
    }
    fclose(fd);

    return 0;
}
