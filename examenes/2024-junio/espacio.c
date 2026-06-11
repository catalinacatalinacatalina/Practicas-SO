#define _POSIX_C_SOURCE 200809L // SOLO EN WSL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#define UMBRAL 300

int recorrer(char *fname);
int recorrer_dir(char *dname)
{
    DIR *dir = opendir(dname);
    if (!dir) {
        perror(dname);
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // me salto las entradas principales
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char subpath[4096];
        snprintf(subpath, sizeof(subpath), "%s/%s", dname, entry->d_name);

        if (recorrer(subpath) == -1) {
            closedir(dir);
            return -1;
        }
    }

    closedir(dir);
    return 0;
}
int recorrer(char *fname)
{
    struct stat st;
    if (lstat(fname, &st) == -1) {
        perror(fname);
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        if (recorrer_dir(fname) == -1)
            return -1;
    } else{
        struct stat st;
        if (lstat(fname, &st) == -1) {
            perror(fname);
            return -1;
        }
        char *nombre = strrchr(fname, '/');
        nombre = nombre ? nombre + 1 : fname;
        char *extension = strrchr(nombre, '.');
        printf("%c\n", extension[1]);
        
        if (strcmp(extension, ".png") == 0){

            char ruta[4096];
            if(st.st_size/(1024)>UMBRAL){
                snprintf(ruta, sizeof(ruta), "./Folder_01/%s", nombre);
            } else{
                snprintf(ruta, sizeof(ruta), "./Folder_02/%s", nombre);
            }
            link(fname, ruta);
        }
    }

    return 0;
}



int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <dir>\n", argv[0]);
        return EXIT_FAILURE;
    }

    recorrer(argv[1]);

    return 0;
}
