#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int setArgs(char* args, char **argv){
    int ret = 0;
    char* it = args;
    char* begin_str = args;
    // Quitar espacios al principio
    while(isspace(*it++));

    while(*it != '\0') {
        if(isspace(*it)){
            if(argv!= NULL){
                *it = '\0';
                argv[ret] = begin_str;
                it++;
                begin_str = it;
                printf("Argv[%d] = '%s'\n", ret, argv[ret]);
            }
            ret++;
        } else{
            it++;
        }
    }

    return ret + 1; // +1 for the NULL terminator
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        perror("El numero de argumentos no es el adecuado\n");
    }

    char args_parsed[6][50];
    printf("Number of args: %d\n", setArgs(argv[1], args_parsed));
    return 0;
}
