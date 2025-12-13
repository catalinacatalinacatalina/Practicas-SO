#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    int n = 0;
    int e_flag = 0;
    int opt;
    
    // Parsear los flags
    while ((opt = getopt(argc, argv, "n:e")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'e':
                e_flag = 1;
                break;
            default:
                fprintf(stderr, "Uso: %s [-n N] [-e] <archivo>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Comprobar que hay un archivo como argumento extra
    if (optind >= argc) {
        fprintf(stderr, "Debe indicar el archivo a mostrar\n");
        exit(EXIT_FAILURE);
    }

    // Abrir el archivo
    int fd = open(argv[optind], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Mover el puntero de fichero según los flags
    off_t offset;
    if (e_flag) {
        // Leer los últimos N bytes
        off_t file_size = lseek(fd, 0, SEEK_END);
        if (file_size == -1) { perror("lseek"); exit(EXIT_FAILURE); }
        offset = (n < file_size) ? file_size - n : 0;
        if (lseek(fd, offset, SEEK_SET) == -1) { perror("lseek"); exit(EXIT_FAILURE); }
    } else {
        // Saltar los primeros N bytes
        if (lseek(fd, n, SEEK_SET) == -1) { perror("lseek"); exit(EXIT_FAILURE); }
    }

    // Leer y escribir byte a byte
    char c;
    ssize_t r;
    while ((r = read(fd, &c, 1)) > 0) {
        if (write(STDOUT_FILENO, &c, 1) != 1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
    if (r < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}
