#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 512

void copy(int fdo, int fdd) {
    unsigned char bloque[BUFSIZE];
    int n_read;
    while ((n_read = read(fdo, bloque, BUFSIZE)) > 0) {
        // Transfer data from the buffer onto the output file
        if (write(fdd, bloque, n_read) < n_read) {
            perror("write");
            exit(1);
        }
    }
    if (n_read < 0) {
        perror("read");
        exit(1);
    }
}

void copy_regular(char *orig, char *dest) {
    int fdor, fddr;
    fdor = open(orig, O_RDONLY);
    if (fdor < 0) {
        perror("open");
        exit(1);
    }
    fddr = open(dest, O_TRUNC | O_CREAT | O_WRONLY, 0644);
    if (fddr < 0) {
        close(fdor);
        perror("open");
        exit(1);
    }
    copy(fdor, fddr);
    close(fdor);
    close(fddr);
}

void copy_link(char *orig, char *dest, int tam) {
    char buffer[tam]; // Buffer to hold the link's path
    int n_read = readlink(orig, buffer, tam - 1); // Read the link's target
    if (n_read < 0) {
        perror("readlink");
        exit(1);
    }
    buffer[n_read] = '\0'; // Null-terminate the string
    // Create a new symbolic link pointing to the same target
    if (symlink(buffer, dest) < 0) {
        perror("symlink");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    struct stat sb;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFLNK:
            printf("symlink\n");
            copy_link(argv[1], argv[2], sb.st_size + 1);
            break;
        case S_IFREG:
            printf("regular file\n");
            copy_regular(argv[1], argv[2]);
            break;
        default:
            printf("unknown file\n");
            break;
    }

    return 0;
}
