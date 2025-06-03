#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

# define BUFSIZE 512
void copy(int fdo, int fdd)
{
	unsigned char bloque[BUFSIZE];
	int n_read;
	while ((n_read = read(fdo, bloque, BUFSIZE)) > 0) {
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
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
		return 1;
	}

	// ESTO ES UN INT!!!
	int source;
	int dest;

	if((source = open(argv[1], O_RDONLY)) == -1) {
		fprintf(stderr, "Error opening source file: %s\n", argv[1]);
		return 1;
	}

	// 0644
	/*El primer dígito (0) indica que no hay bits especiales (setuid, setgid, sticky).
	* segundo dígito (6) indica lectura y escritura para el dueño (usuario).
	* El tercer dígito (4) indica solo lectura para el grupo.
	* El cuarto dígito (4) indica solo lectura para otros (resto de usuarios).
	*/
	if ((dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
		fprintf(stderr, "Error opening destination file: %s\n", argv[2]);
		return 1;
	}
	copy(source, dest);

	close(source);
	close(dest);

	return 0;
}

