/*
	Programa que reciba dos argumentos: el primero sera un directorio
	y el segundo un numero en octal de 4 digitos que transformaremos en tipo numerico (strol).
	Este numero se usara para cambiar los permisos de todos los archivos 
	que se encuentren en el direcotio
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  // open
#include <stdio.h>  // printf, perror
#include <errno.h>  // perror
#include <stdlib.h> // exit
#include <unistd.h> // read, close
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        fprintf(stderr, "Error: Numero de parametros incorrecto");
        exit(2);
    }

    setbuf(stdout, NULL);

    char ruta[512];
    DIR *dir;
    unsigned int permisos;
    struct dirent *ent;
    struct stat st;

    if ((dir = opendir(argv[1])) == NULL)
    {
        perror("Error al abrir el directorio\n");
        exit(1);
    }

    umask(0);
    permisos = strtol(argv[2], NULL, 8);

    while ((ent = readdir(dir)) != NULL)
    {
        sprintf(ruta, "%s/%s", argv[1], ent->d_name);

        if (stat(ruta, &st) < 0)
        {
            perror("Error al recuperar el archivo\n");
            exit(1);
        }

        if (S_ISREG(st.st_mode))
        {
            char ruta2[512];
            sprintf(ruta2, "%s", ent->d_name);

            printf("%s || %o\n", ruta2, st.st_mode);

            if (chmod(ruta, permisos) < 0)
            {
                perror("Error al cambiar los permisos\n");
                exit(1);
            }
            else
            {
                stat(ruta, &st);
                printf("%s || %o\n", ruta, st.st_mode);
            }
        }
    }
}
