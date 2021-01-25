// Programa para MODIFICAR banderas

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 

int main(int argc, char * argv[]){
	const char ARCHIVO[] = "archivo.txt";
	int banderas;
	int fd;

	fd = open(ARCHIVO, O_SYNC, O_RDWR);
	if(fd < 0){
		perror("Open error");
		exit(1);
	}

	// Conseguimos las banderas
	banderas = fcntl(fd, F_GETFL);

	if(banderas == -1){
		perror("fcntl error");
		exit(1);
	}

	// Comprobamos si esa abierto en modo O_SYNC
	if(banderas & O_SYNC){
		printf("Las escrituras son sincronizadas\n");
		exit(1);
	}

	if(banderas & O_APPEND){
		printf("Modo O_APPEND activado\n");
		exit(1);
	}

	printf("----------------------------------\n");
	// Cambiamos una de las banderas de estado
	// Hacemos un OR en vez de un AND porque con el AND quitaria TODAS LAS BANDERAS ;(
	banderas |= O_APPEND; // Esto es igual a banderas = banderas | O_APPEND!!
	if(fcntl(fd, F_SETFL, banderas) == -1){ // SETFL PARA CAMBIAR LAS BANDERAS
		perror("Error en fcntl - cambio de banderas");
		exit(1);
	}

	// Comprobams
	if(banderas & O_APPEND){
		printf("Modo O_APPEND activado\n");
		exit(1);
	}

	close(fd);
}