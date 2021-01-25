#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



int main(int argc, char * argv[]){

	//Comprobacion de errores

  if(argc != 2){
      fprintf(stderr, "Error: numero de argumentos incorrectos");
      exit(2);
  }


	struct stat st;


  if(stat(argv[1], &st) < 0) {
      perror("Error en el stat");
      exit(1);
  }

	//Comprobar si es un directorio

	if(S_ISDIR(st.st_mode)){

		printf("Fecha de ultima modificacion %lu\n", st.st_mtime);

	}else{
		
		printf("Este archivo no es un directorio\n");
	}



}
