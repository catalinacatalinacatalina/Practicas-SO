#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>

/* LOADSTR: 
	lee la siguiente cadena terminada en '\0' de un fichero, 
	la copia en memoria dinámica y devuelve un puntero a esa copia.
*/
 char *loadstr(FILE *file) {
    int length = 0;
    int space;

    // 1- guardar donde empieza la palabra
    long start = ftell(file);

    // 2- leer caracter a caracter la palabra (separadas por '\0') Y contamos letras
    while ((space = getc(file)) != '\0' && space != EOF) { length++; }

    // 3- salirse si no hay nada en el fichero
    if (space == EOF && length == 0) {  return NULL; }

	// 4- volver al principio del fichero
    fseek(file, start, SEEK_SET);

    // 5- reservar espacio segun letras paso 2
    char* str;
    if ((str  = (char *) malloc((length + 1) * sizeof(char)))== NULL) {
        err(3, "Error al reservar memoria");
    }

    // 6- leer cadena completa -> fread(arch_salida, tamaño, nletras, archivo_entrada)
    fread(str, sizeof(char), (length + 1), file);

    // 7- devolver ptr de la cadena leida
    return str;	
}


int main(int argc, char *argv[])
{

	FILE* file = NULL;
	char* c = NULL;

	// Argumentos siempre seran 2
	if (argc!=2) {
		printf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	// abrimos archivo
	if ((file = fopen(argv[1], "r")) == NULL){
		err(2,"The input file %s could not be opened",argv[1]);
		err(3, "Error al reservar memoria");
	}

	// para cada palabra, la guardamos en c
	while ((c = loadstr(file)) != NULL) {
		printf("%s\n", c);
		free(c);
	}

	// cerramos archivo
	fclose(file);
	return 0;
}
