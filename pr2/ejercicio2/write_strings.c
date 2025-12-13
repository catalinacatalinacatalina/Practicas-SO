#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>


int write_strings(FILE *file, char *strings[], int num_strings)
{
    int i;
    for (i = 0; i < num_strings; i++) {
        size_t len = strlen(strings[i]) + 1;

        // Escribe la cadena completa, incluyendo el '\0'
        if (fwrite(strings[i], sizeof(char), len, file) != len) {
            perror("Error al escribir en el archivo");
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    /* Verifica que el nombre del archivo y al menos una cadena se hayan pasado como argumentos */
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <nombre_archivo> <cadena1> [cadena2] ...\n", argv[0]);
        exit(1);
    }

    FILE *file = fopen(argv[1], "wb");  // Abre el archivo en modo binario para escritura
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    /* Llama a write_strings para escribir las cadenas en el archivo */
    if (write_strings(file, &argv[2], argc - 2) != 0) {
        fprintf(stderr, "Error al escribir las cadenas en el archivo.\n");
        fclose(file);
        exit(1);
    }

    fclose(file);
    printf("Cadenas escritas correctamente en el archivo '%s'.\n", argv[1]);
    return 0;
}