#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <getopt.h>
#include <dirent.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if(argc<3){
        perror("error");
        return EXIT_FAILURE;
    }
    int opt;
    int nbytes= NULL;
    char* outFileName =stdout;
    while ((opt = getopt(argc, argv, "n:o:")) != -1) {
        switch (opt) {
            case 'o':
            outFileName = optarg;
            break;
            case 'n':
            nbytes = atoi(optarg);
            break;
            default:
                fprintf(stderr, "Uso: %s -n <numBytes> [-o <outFileName>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    FILE * salida;
    if(outFileName!=stdout){
        salida = fopen(outFileName, "w");
    }

    DIR *dir = opendir("./dir1");
    if (!dir) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    // Bucle que recorre todos los ficheros del directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Obtener información del fichero
        struct stat st;
        if (stat(entry->d_name, &st) == -1) {
            perror("stat");
            continue;
        }

        // Comprobar si es un fichero regular
        if (S_ISREG(st.st_mode)) {
            printf("Archivo: %s, tamaño: %ld bytes\n", entry->d_name, st.st_size);

            //Aquí podrías abrir el fichero y leer hasta nbytes
            FILE *f = fopen(entry->d_name, "rb");
            int c, ret, counter=0;
            while (counter<nbytes && (c = getc(f)) != EOF) {
                if(outFileName)
                    ret=putc((unsigned char) c, salida);
                else
                    ret = putc((unsigned char)c, stdout);

                if (ret==EOF){
                    fclose(f);
                    err(3,"putc() failed!!");
                }
            }
            fclose(f);
        }
    }

    closedir(dir);
        
    return EXIT_SUCCESS;
}
