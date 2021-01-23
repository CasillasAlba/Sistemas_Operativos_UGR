// Programa que recibe dos argumentos: primero el nombre de un fichero en el cual vamos a escribir
// el nombre del segundo fichero, solamente si este es regular y tiene mas de 100 Bytes.

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]){
	if(argc != 3){
		fprintf(stderr, "Error: Numero de parametros incorrecto\n");
		exit(2);
	}

	int fd;

	if((fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR)) < 0){
		perror("Error al abrir el descriptor de fichero");
		exit(1);
	}

	struct stat st;
	stat(argv[2], &st);

	// Si el archivo pasado como SEGUNDO arguento es regular
	if(S_ISREG(st.st_mode)){
		// Si tiene mas de 100 Bytes
		if(st.st_size > 100){
			// Escribimos en fd el valor de la variable argv[2] (nombre del 2ndo parametro)
			if(write(fd, argv[2], strlen(argv[2])*sizeof(char)) < 0){
				perror("Error en el wirte");
				exit(1);
			}
		}
	}

	close(fd);
}
