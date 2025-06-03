#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
	FILE* file = NULL;
	char* fichero = argv[0];
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
		exit(1);
	}

	if((file = fopen(argv[1], "w")) == NULL) {
		err(2, "The input file %s could not be opened", argv[1]);
	}

	int i = 2;
	while(i< argc) {
		if (fwrite(argv[i], sizeof(char), strlen(argv[i]) + 1, file) < strlen(argv[i]) + 1) {
			err(3, "Error writing to file %s", argv[1]);
		}
		i++;
	}
	fclose(file);

	return 0;
}
