#define _GNU_SOURCE	// para el WSL
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
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
	unsigned char bloque[512];

	int n_read = 0;
	if((fdor = open( orig, O_RDONLY))<0){
		exit(1);
	}
	if((fddr = open( dest, O_WRONLY|O_CREAT|O_TRUNC, 0644))<0){
		close(fdor);
		exit(1);
	}

	while( (n_read = read(fdor, bloque, sizeof(bloque))) >0){
		if(n_read<0){
			perror("read");
			close(fdor);
			exit(1);
		}

		if(write(fddr, bloque, n_read)<n_read){ //ssize_t write(int fildes, const void *buf, size_t nbyte);
			perror("write");
			close(fdor);
			close(fddr);
			exit(1);
		}
	}

	close(fdor);
	close(fddr);


    
}

void copy_link(char *orig, char *dest) {
	struct stat info;

	if (lstat(orig, &info) == -1) {
		perror("lstat");
		exit(EXIT_FAILURE);
	}

	size_t tam = malloc(info.st_size + 1);
	char * buffer = tam;

	int n_read =  readlink(orig, buffer, tam);

	if(n_read < 0){
		perror("readlink");
		EXIT_FAILURE;
	}

	buffer[n_read] = '\0';

	if(symlink(buffer, dest) < 0){
		perror("symlink");
		EXIT_FAILURE;
	}


}

int main(int argc, char *argv[]) {
    struct stat sb;

	if(argc<2){
		perror("error de entrada");
		exit(1);
	}

    if (lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFLNK:
            printf("symlink\n");
            copy_link(argv[1], argv[2]);
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
