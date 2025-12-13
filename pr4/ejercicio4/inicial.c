#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <linux/stat.h>

void solucionA(int fd1) {
    int i;
    char buffer[6];

    // Escribir los 5 ceros iniciales en el archivo
    write(fd1, "00000", 5);

    for (i = 1; i < 10; i++) {
        if (fork() == 0) {
            /* Código del hijo */
            // Generar la cadena correspondiente al hijo
            sprintf(buffer, "%d%d%d%d%d", i, i, i, i, i);

            // Calcular la posición de escritura
            int pos = i * 5;

            // Escribir en la posición correspondiente
            pwrite(fd1, buffer, 5, pos);

            close(fd1); // Cerrar el descriptor de archivo
            exit(EXIT_SUCCESS);
        }
    }

    // Esperar a que todos los hijos terminen
    while (wait(NULL) != -1);
}

void solucionB(int fd1) {
    int i;
    char buffer[6];

    for (i = 0; i < 10; i++) {
        if (fork() == 0) {
            /* Código del hijo */
            // Generar la cadena correspondiente al hijo
            sprintf(buffer, "%d%d%d%d%d", i, i, i, i, i);

            // Calcular la posición de escritura
            int pos = i * 10 + 5;

            // Escribir en la posición correspondiente
            pwrite(fd1, buffer, 5, pos);

            close(fd1); // Cerrar el descriptor de archivo
            exit(EXIT_SUCCESS);
        } else {
            /* Código del padre */
            // Generar los cinco ceros
            sprintf(buffer, "00000");

            // Calcular la posición de escritura
            int pos = i * 10;

            // Escribir los ceros en la posición correspondiente
            pwrite(fd1, buffer, 5, pos);
        }
    }

    // Esperar a que todos los hijos terminen
    while (wait(NULL) != -1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <A|B>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Crear o truncar el archivo
    int fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd1 < 0) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "A") == 0) {
        printf("Ejecutando solución A...\n");
        solucionA(fd1);
    } else if (strcmp(argv[1], "B") == 0) {
        printf("Ejecutando solución B...\n");
        solucionB(fd1);
    } else {
        fprintf(stderr, "Opción inválida. Use 'A' o 'B'.\n");
        close(fd1);
        exit(EXIT_FAILURE);
    }

    // Mostrar el contenido del archivo
    lseek(fd1, 0, SEEK_SET);
    printf("El contenido del archivo es:\n");
    char c;
    while (read(fd1, &c, 1) > 0) {
        printf("%c", c);
    }
    printf("\n");

    close(fd1);
    return 0;
}

