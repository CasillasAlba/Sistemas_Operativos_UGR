/*
	El programa se llamara verEnlaces.c y tendra como argumento
	la ruta de un archivo. El programa debera explorar en su
	totalidad el arbol de archivos (RECURSIVIDAD)
	y mostrar en pantalla aquellas rutas que sean un enlace
	duro dado como argumento.

	Minimice la busqueda haciendo uso del valor que tome el
	contrador de enlaces para la ruta del argumento.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

void Busca(char *nombreDir, ino_t inodo, int enlacesRestantes){
	DIR * directorio;
	struct dirent * entrada;
	struct stat st;
	char ruta[4096];

	if((directorio = opendir(nombreDir)) == NULL){
		perror("Error al abrir el directorio");
		exit(1);
	}

	while((entrada = readdir(directorio)) != NULL && enlacesRestantes){
		sprintf(ruta, "%s/%s", nombreDir, entrada->d_name);

		if(stat(ruta,&st) < 0){
			perror("Error en el stat");
			//exit(1);
			continue; // Prueba con el siguiente
		}

		if(S_ISDIR(st.st_mode)){ // Si la entrada es un directorio
			if(strcmp(entrada->d_name, ".") && strcmp(entrada->d_name, "..")){
				Busca(ruta, inodo, enlacesRestantes);
			}

		}else{ // Si no es un directorio, comprueba si la ruta es la dada por el argumento
			if(st.st_ino == inodo){
				printf("Numero de enlaces: %d - Ruta: %s\n", enlacesRestantes, ruta);
				enlacesRestantes--;
			}
		}
	}

	closedir(directorio);
}


int main(int argc, char * argv[]){
	if(argc != 2){
		fprintf(stderr, "Error: Numero de parametros incorrecto\n");
		exit(2);
	}

	struct stat st;
	if(stat(argv[1], &st) < 0){
		perror("Error en el stat");
		exit(1);
	}

	// Buscara por todo el arbol de directorios
	Busca("/home/alba", st.st_ino, st.st_nlink);
}