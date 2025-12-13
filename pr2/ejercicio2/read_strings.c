#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)

{	/* To be completed */
	int lenght = 0;
	int space;

	long start = ftell(file);

	while((space = getc(file)) != '\0' && space != EOF){
		lenght++;
	}

	if (space == EOF && lenght == 0){
		return NULL;
	}

	fseek(file, start, SEEK_SET);

	char* str = (char *) malloc((lenght + 1) * sizeof(char));
	    if (str == NULL) {
        err(3, "Error al reservar memoria");
		}

	fread(str, sizeof(char), (lenght + 1), file);

	return str;	
}

int main(int argc, char *argv[])
{
	/* To be completed */
	FILE* file = NULL;
	char* c = NULL;


	if (argc!=2) {
		printf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	if ((file = fopen(argv[1], "r")) == NULL){
		err(2,"The input file %s could not be opened",argv[1]);
		err(3, "Error al reservar memoria");
	}

	else{

	while ((c = loadstr(file)) != NULL) {
		printf("%s\n", c);
		free(c);
	}
	}

	fclose(file);
	return 0;
}
