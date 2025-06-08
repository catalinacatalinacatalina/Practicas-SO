#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

int main(void)
{
    int fd1,fd2,i,pos;
    char c;
    char buffer[6];

    // Abre/crea archivo "output.txt" - permisos: lectura y escritura para el usuario, trunca
    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);

    // Escribe "00000"
    write(fd1, "00000", 5);

    for (i=1; i < 10; i++) {
        // Guarda la posición actual del puntero de archivo
        pos = lseek(fd1, 0, SEEK_CUR);
        if (fork() == 0) {
            /* Child */
            // Escribe en buffer 
            sprintf(buffer, "%d", i*11111);

            // Coloca puntero de archivo en posición guardada
            lseek(fd1, pos, SEEK_SET);

            // Escribe el contenido del buffer (5 caracteres) en el archivo
            write(fd1, buffer, 5);

            // Cierra el archivo y termina el hijo
            close(fd1);
            exit(0);
        } else {
            /* Parent */
            // Avanza el puntero de archivo 5 posiciones
            lseek(fd1, 5, SEEK_CUR);

            // Espera a que terminen todos los hijos
            while (wait(NULL) != -1);
        }
    }

    // Vuelve al principio del archivo
    lseek(fd1, 0, SEEK_SET);

    // imprimir
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}