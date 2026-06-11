//cuestion1
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

int leer_directorio(char *fname, int nbits, char* fout){
    DIR *dir = opendir(fname);
    if (!dir) {
        perror(fname);
        return -1;
    }

    int fdout = -1;
    if (fout != NULL) {
        fdout = open(fout, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fdout == -1) {
            perror("open");
            closedir(dir);
            return -1;
        }
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char subpath[4096];
        snprintf(subpath, sizeof(subpath), "%s/%s", fname, entry->d_name);

        struct stat st;
        if (lstat(subpath, &st) == -1) {
            perror(subpath);
            return -1;
        }

        if (!S_ISREG(st.st_mode)) {
            // LEER NUMBYTES
            int fich = open(subpath, O_RDONLY);
            if (fich == -1) {
                perror(subpath);
                continue;
            }

            char c;
            ssize_t r;
            int ctr= nbits;
            int fd = (fout == NULL) ? STDOUT_FILENO : fdout;

            while ((r = read(fich, &c, 1)) > 0 && ctr>0) {
                if (write(fd, &c, 1) != 1) {
                    perror("write");
                    close(fich);
                    if (fdout != -1) close(fdout);
                    closedir(dir);
                    exit(EXIT_FAILURE);
                }
                ctr--;
            }
            write(fd, "\n", 1);  // después del bucle de lectura de cada fichero

            close(fich);
        }
        
    }
    if (fdout != -1)
        close(fdout);
    closedir(dir);
    return 0;

}



int main(int argc, char *argv[]) {
    int opt;
    int numBytes = -1;
    char* outfile = NULL;
    while ((opt = getopt(argc, argv, "n:o:")) != -1) {
        switch (opt) {
            case 'n':
                numBytes = atoi(optarg);
                break;
            case 'o':
                outfile = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (numBytes== -1) {
        perror("usage");
        return EXIT_FAILURE;
    }

    leer_directorio(".", numBytes, outfile);


    return EXIT_SUCCESS;
}
