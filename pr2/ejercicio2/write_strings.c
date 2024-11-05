#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
	
	FILE* file=NULL;


	if (argc<2) {
		fprintf(stderr,"Usage: %s <file_name>,<String1>,<String2>,<String3...>\n",argv[0]);
		exit(1);
	}
	if ((file = fopen(argv[1], "w")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	int i = 2;
	while(i < argc){
		fwrite(argv[i],sizeof(char),strlen(argv[i])+1,file);
		i++;
	}

	return 0;
}
