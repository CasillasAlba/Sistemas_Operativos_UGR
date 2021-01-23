// Programa que acepte como argumento el nombre de un archivo (pathname) y lo lee 
// en bloques de tamanio 80 BYTES.
// En salida.txt se esscribira la informacion entre strings que indique el numero de bloque
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int cont = 1, leidos;
    int filein, fileout;
    char cadena[30];
    char cad_bloque[40];
    char caracter[312];
    int num_char = 1;

    if (argc != 2)
    {
        fprintf(stderr, "Error: Numero de parametros incorrecto");
        exit(2);
    }

    if ((filein = open(argv[1], O_RDONLY)) < 0)
    {
        perror("No se puedo abrir el fichero");
        exit(1);
    }
    
    fileout = open("salida", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fileout < 0)
    {
        perror("Error al abrir o crear el archivo de salida\n");
        exit(1);
    }

    int bloques = 1;
    char mensaje[512];
    while ((leidos = read(filein, caracter, 80)) > 0)
    {
        //printf("%s\n", caracter);
        sprintf(mensaje, "El bloque numero %i de 80 bytes:\n", bloques);
        if ((write(fileout, mensaje, sizeof(char) * 34)) > 0)
        {
            if ((write(fileout, caracter, leidos)) > 0)
            {
                if ((write(fileout, "\n", sizeof(char) * 2)) > 0)
                {
                    bloques++;
                }
                else
                {
                    perror("Error al escribir salto de linea");
                    exit(1);
                }
            }
            else
            {
                perror("Error al escribir bloque de 80 B");
                exit(1);
            }
        }
        else
        {
            perror("Error al escribir mensaje pre-bloque");
            exit(1);
        }
    }

    close(filein);
    close(fileout);
    return 0;
}
