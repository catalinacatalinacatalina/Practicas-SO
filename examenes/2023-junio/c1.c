#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

int scan_dir_entries(char *fname, char**argv);

// entro en el dir
int scan_dir(char *fname, char** argv)
{
    struct stat st;
    if (lstat(fname, &st) == -1) {
        perror(fname);
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
        printf("DIRECTORIO %s\n", fname);

        if (scan_dir_entries(fname, argv) == -1)
            return -1;
    }

    return 0;
}


// miro cada una de las entradas del dir
int scan_dir_entries(char *dname, char** argv)
{
    DIR *dir = opendir(dname);
    if (!dir) {
        perror(dname);
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            printf("%s\n", entry->d_name);
            continue;
        }

        char subpath[4096];
        snprintf(subpath, sizeof(subpath), "%s/%s", dname, entry->d_name);
        printf("%s\n", entry->d_name);
        char** argumentos = malloc(3*sizeof(char)*10); // max 10 elementos cada palabra
        pid_t pid = fork();
        if(pid==0){
            // argumentos del nuevo programa
            argumentos[0] = argv[0];
            argumentos[1] = "-R";
            argumentos[2] = subpath;
            
            execvp(argumentos[0], argumentos);

        } else{
            int status;
            waitpid(pid, &status, 0);
            free(argumentos);

        }
    }

    closedir(dir);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    int run_in_background = 0;
    char *directorio = NULL;

    while ((opt = getopt(argc, argv, "R:")) != -1) {
        switch (opt) {
            case 'R':
                directorio = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s \n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(directorio == NULL){
        directorio =".";
    }

    if(scan_dir(directorio, argv) == -1) return EXIT_FAILURE;

    return EXIT_SUCCESS;

}