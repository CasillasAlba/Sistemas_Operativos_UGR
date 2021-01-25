/* 
			PROYECCION
	Programa que coja dos archivos y los copie 
	utilizando proyecciones de memoria
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 

int main(int argc, char * argv[]){
	// VARIABLES
	int fd_leer, fd_escritura;
	struct stat informacion;
	int tamanio_fichero;
	char * p_entrada;
	char * p_salida;

	// Comprobacion de argumentos (main + 2 ficheros)
	if(argc != 3){
		fprintf(stderr, "Numero de parametros incorrecto\n");
		exit(2);
	}

	if((fd_leer = open(argv[1], O_RDONLY, S_IRUSR | I_IWUSR)) < 0){
		perror("Error al abrir el primer archivo");
		exit(1);
	}

	if((fd_escritura = open(argv[2], O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | I_IWUSR)) < 0){
		perror("Error al abrir el segundo archivo");
		exit(1);
	}

	/* NECESITAMOS:
		Tamanio del fichero
	*/

	// Cogemos la informacion del primer archivo
	if(lstat(argv[1], &informacion) < 0){
		perror("Error al intentar acceder a los atributos del archivo");
		exit(1);
	} 
	
	tamanio_fichero = informacion.st_size;

	// Por si acaso el fichero de salida ya esta escrito, lo truncamos
	ftruncate(fd_escritura, tamanio_fichero);

	// Creamos las proyecciones de memoria
	// LOS MMAP SE GUARDAN EN UN PUNTERO A UN CHAR -> PORQUE LA PROYECCION
	// DE MEMORIA ES UN PUNTERO APUNTANDO AL ARCHIVO
	// HAY QUE HACERLE UN CASTING PORQUE MMAP DEVUELVE UN VOID!!
	p_entrada = (char*)mmap(NULL, tamanio_fichero, PROT_READ, MAP_SHARED, fd_leer, 0);

	if(p_entrada == MAP_FAILED){
		perror("Fallo en el mapeo");
		exit(1);
	}

	p_salida = (char*)mmap(NULL, tamanio_fichero, PROT_WRITE,MAP_SHARED, fd_escritura, 0);

	if(p_salida == MAP_FAILED){
		perror("Fallo en el mapeo");
		exit(1);
	}

	// Hacemos la copia (copiamos la memoria de los punteros)
	// Usamos memcpy porque esta funcion copia areas de memoria
	memcpy(p_salida, p_entrada, tamanio_fichero);

	close(fd_leer);
	close(fd_escritura);
}