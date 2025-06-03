#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

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
{
	int len = 0;
    long start_pos = ftell(file);
    int ch;

    // 1. Contar la longitud de la cadena (hasta '\0')
    while ((ch = fgetc(file)) != EOF && ch != '\0') {
        len++;
    }

    if (ch == EOF && len == 0) return NULL; // Fin de archivo

    // 2. Restaurar el puntero a la posición original
    fseek(file, start_pos, SEEK_SET);

    // 3. Reservar memoria suficiente (+1 para el '\0')
    char* str = malloc(len + 1);
    if (!str) return NULL;

    // 4. Leer la cadena completa
    fread(str, 1, len + 1, file); // len caracteres + '\0'
    return str;	
}

int main(int argc, char *argv[])
{
	FILE *file = NULL;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
		exit(1);
	}
	if ((file = fopen(argv[1], "r")) == NULL) {
		err(2, "The input file %s could not be opened", argv[1]);
	}

	while(1){
		char* str = loadstr(file);
        if (!str) break; // Fin de archivo

        printf("%s\n", str);
        free(str);
	}
	fclose(file);
	return 0;
}
