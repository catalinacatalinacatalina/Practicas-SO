#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#define BUFSIZE 512
void copy(int fdo, int fdd)
{
	unsigned char bloque[BUFSIZE];
	int n_read;
	while ((n_read = read(fdo, bloque, BUFSIZE)) > 0) {
	/* Transfer data from the buffer onto the output file */
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

int main(int argc, char *argv[])
{
	if(argc < 2){
		perror("entrada");
		exit(1);
	}
	int fdor,fddr;
	fdor = open(argv[1],O_RDONLY);
	if (fdor < 0){
		perror("open"); exit(1);
}
	fddr = open(argv[2],O_TRUNC|O_CREAT|O_WRONLY,0644);
	if (fddr < 0){
		close(fdor);
		perror("open"); exit(1);
}
	copy(fdor,fddr);
	close(fdor);
	close(fddr);
	return 0;
}
