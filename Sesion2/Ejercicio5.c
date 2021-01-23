// Programa que recibe como argumento del nombre de un directrio.
//Añade permisos de escritura para otros, y si ya los tiene los quita

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
	struct dirent * entrada;
	struct stat st;
	char ruta[512];
	mode_t permisos;

	if((directorio = opendir(argv[1])) == NULL){
		perror("Error al abrir el directorio");
		exit(1);
	}

	while((entrada = readdir(directorio)) != NULL){

		sprintf(ruta, "%s/%s", argv[1], entrada->d_name);

		if(stat(ruta,&st) < 0){
			perror("Error en el stat");
			exit(1);
		}

		if((st.st_mode & S_IWOTH) == S_IWOTH){ // Tiene permisos
			// Los quita
			permisos = st.st_mode & (~S_IWOTH);

		}else{
			// Los aniade
			permisos = st.st_mode | S_IWOTH;
		}
		
		if(chmod(ruta,permisos) < 0){
			perror("Error al dar los permisos");
			exit(1);
		}
	}
	closedir(directorio);
}
