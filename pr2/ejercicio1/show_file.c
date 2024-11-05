#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
int main(int argc, char* argv[]) {
	FILE* file=NULL;
	size_t         ret;
	unsigned char*  buffer;


	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	while(fread(buffer, 1, 1, file) > 0){			//Fread char by 1 byte by 1 elem while fread() returns > 0 
	int ret = fwrite(buffer,1,1,stdout);			//Fwrite char by 1 byte by 1 elem of buffer
		if(ret != 1){
			printf(err);
			exit(1);
		}
	}
	

	fclose(file);
	return 0;
}
