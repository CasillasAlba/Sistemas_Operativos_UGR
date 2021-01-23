//Programa que muestre el i-nodo y el tamaño de un archivo pasado por argumento

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char * argv[]){

	if(argc != 2){
		fprintf(stderr, "Error: Número de parámetros incorrecto\n");
		exit(2);
	}	

	struct stat st;
	stat(argv[1], &st);

    printf("i-nodo: %ld  y tamanio: %ld\n", st.st_ino, st.st_size);

}
