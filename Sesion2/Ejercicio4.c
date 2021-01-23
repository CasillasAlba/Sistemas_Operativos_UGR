// Programa que recibe como argumento del nombre de un directrio.
//Añade permisos de escritura para otros a todos los ficheros regulares contenidos en dicho directorio.

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char*argv[]){
	if(argc != 2){
		fprintf(stderr, "Error: Numero de parametros incorrecto\n");
		exit(2);
	}

	DIR * directorio;
	struct dirent * entrada; //Cada una de las cosas que tiene el directorio

	// Primero abrimos el directorio
	if((directorio = opendir(argv[1])) == NULL){
		perror("Error al abrir el directorio");
		exit(1);
	}


	/*
		El uso de chmod es chmod("fichero", permisos)... 
		Sin embargo, si dentro del directorio que estamos recorriendo hay
		otro directorio, tenemos que acceder a él y el chmod fallaria.

		¿SOLUCIÓN? -> Guardar LA RUTA

	*/

	struct stat st;
	char ruta[512]; //Tamanio suficiente para almacenar la ruta

 	while((entrada = readdir(directorio)) != NULL){

		//AQUÍ CREAMOS LA RUTA!!!! Concatenamos nombre_dir/nombre_fichero
		sprintf(ruta, "%s/%s", argv[1], entrada->d_name);

		// Para cada fichero, vemos si es regular
		// Por eso debemos de hacer el stat de LA RUTA
		if(stat(ruta,&st) < 0){
			perror("Error en el stat");
			exit(1);
		}

		if(S_ISREG(st.st_mode)){
			// CONCATENAMOS permisos para que no se machaquen
			if(chmod(ruta, st.st_mode | S_IWOTH) < 0){
				perror("Error al dar los permisos");
				exit(1);
			}
		}

	}

	closedir(directorio);

}


