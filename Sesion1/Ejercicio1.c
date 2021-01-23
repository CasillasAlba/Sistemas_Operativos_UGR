//Programa que copie lo que hay en el fichero datos.txt en copia.txt
//Para saber los includes que usar, en la terminal $ man 2 open, $ man 2 read....


//echo "hola caracola :)" > datos.txt  SE PUEDE USAR
//Para compilar: gcc Programa1.c -o ej
//Para ejecutar: ./ej


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]){
	
	if(argc != 3){
		/*

			perror("Numero de parámetros incorrecto");
			NO escribimos perror porque no ha habido una llamada del sistema

		*/

		fprintf(stderr, "Error: Número de parámetros incorrecto\n");
		exit(2);
	}

	int fd, fd_2;
	char valor; // Variable que se va a leer

	// Abrimos "datos.txt" para lectura

	if((fd = open(argv[1], O_RDONLY)) < 0){
		perror("error al abrir datos.txt");
		exit(1);
	}

	// Abrimos "copia.txt" con permisos de escritura. Si no existe se crea. O_TRUNC será por si volvemos a ejecutar habiendo modificado datos.txt, borramos lo que hay y volvemos a copiar

	if((fd_2 = open(argv[2], O_WRONLY | O_CREAT| O_TRUNC, S_IWUSR | S_IRUSR)) < 0){
		perror("error al abrir copia.txt");
		exit(1);
	}

	// Mientras podamos leer de datos.txt, escribiremos en copia.txt
	while((read(fd,&valor,sizeof(char))) > 0){
		if(write(fd_2,&valor,sizeof(char)) < 0){
			perror("Error al escribir");
			exit(1);
		}

	}

	close(fd);
	close(fd_2);

}