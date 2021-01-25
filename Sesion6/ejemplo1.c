// Programa que comprueba banderas de creacion de un archivo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 

int main(int argc, char * argv[]){
	const char ARCHIVO[] = "ejemplo1.c";
	// Un numero ENTERO nos codifica todas las banderas del archivo
	int banderas;
	int fd;

	// Abrimos el archivo
	fd = open(ARCHIVO, O_SYNC, O_RDWR);

	banderas = fcntl(fd, F_GETFL); // F_GETFL obtiene las banderas de fd

	if(banderas == 1){
		perror("fcntl error");
		exit(1);
	}

	// Comprobamos si esta abierto en modo O_SYNC
	// Hacemos un AND(&) para que el archivo cumpla el O_SYNC
	// 1 1 0 1 1 1 0 0 <- Banderas
	// 0 0 0 1 0 0 0 0 <- O_SCYNC (1bit)
	// -------------------
	// 0 0 0 1 0 0 0 0 -> Si el bit de O_SYNC esta encendido (1) el resultado
	// final tambien estara a 1 y por ende SI tiene esa bandera activada
	if(banderas & O_SYNC){
		printf("Las escrituras son sincronizadas\n");
	}

	printf("Banderas del archivo '%s': %i\n", ARCHIVO, banderas);
}