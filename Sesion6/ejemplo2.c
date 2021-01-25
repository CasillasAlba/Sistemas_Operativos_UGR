// Programa que comprueba PERMISOS de un archivo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 

int main(int argc, char * argv[]){
	const char ARCHIVO[] = "archivo.txt";
	int banderas, modoAcceso;
	int fd;

	// Abrimos el archivo
	fd = open(ARCHIVO, O_RDWR);
	if(fd < 0){
		perror("Open error");
		exit(1);
	}

	// Conseguimos las banderas
	banderas = fcntl(fd, F_GETFL);

	if(banderas == -1){
		perror("fcntl error");
	}

	// Obtenemos el modo de acceso -> Para obtener PERMISOS!!!!
	// Es decir, si queremos solamente saber los bits de un permiso
	// hay que hacer un AND con la mascara O_ACCMODE

	modoAcceso = (banderas & O_ACCMODE);

	// Ahora coge los bits exactos del modo de apertura
	if(modoAcceso == O_WRONLY || modoAcceso == O_RDWR){
		printf("El archivo esta abierto para escritura\n");
	}else{
		printf("No esta abierto ni para escritura ni lectura\n");
	}

	close(fd);
}