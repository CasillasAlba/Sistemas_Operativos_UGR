// Programa que obtenga el menor y mayor tamanio en bytes de las entradas del directorio actual
// Creamos DOS hijos: el primero que reciba desde min hasta mitad y el segundo desde mitad+1 hasta max
// Escriben en el pipe el inodo y el tamanio de las entradas comprendidas en es rango
// El padre leera el inodo y el tamanio escrito por los hijos y comprobara si dicho inodo tiene el tamanio indicado

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

// EL ORDEN IMPORTA
// Los datos se leen/escriben en el orden declarados
struct Info{
	ino_t inodo;
	off_t size;
};

int main(){
	int fd[2];
	pid_t pid;
	DIR * directorio;
	struct dirent * entrada;
	struct stat st;
	off_t min, max, mitad, max_aux, min_aux;
	struct Info informacion;
	ino_t inodo;
	off_t size;

	if((directorio = opendir(".")) == NULL){
		perror("Error al abrir el directorio");
		exit(1);
	}

	// Para no dar valores arbitrarios iniciales a min, max
	// le asignaremos el tamanio en Bytes de la priemra entrada
	if((entrada = readdir(directorio)) == NULL){
		perror("Error al leer la primera entrada");
		exit(1);
	}

	if(stat(entrada->d_name,&st) < 0){
		perror("Error en el primer stat");
		exit(1);
	}

	min = max = st.st_size;

	while((entrada = readdir(directorio)) != NULL){
		if(stat(entrada->d_name,&st) < 0){
			perror("Error al hacer el stat");
			exit(1);
		}

		if(st.st_size > max){
			max = st.st_size;
		}

		if(st.st_size < min){
			min = st.st_size;
		}
	}

	mitad = (min + max)/2;
	printf("tamanio min: %ld y tamanio max: %ld\n", min, max);
	closedir(directorio);

	// PRIMER HIJO
	if(pipe(fd) < 0){
		perror("Error al abrir el pipe");
		exit(1);
	}

	if((pid = fork()) < 0){
		perror("Error al crear el hijo");
		exit(1);
	}else if(pid == 0){ // HIJO 1
		close(fd[0]); // Cerramos CAUCE LECTURA
		max_aux = mitad;

		if((directorio = opendir(".")) == NULL){
			perror("Error al abrir directorio - 1er hijo");
			exit(1);
		}

		while((entrada = readdir(directorio)) != NULL){
			if(stat(entrada->d_name,&st) < 0){
				perror("Error al hacer el stat - 1er hijo");
				exit(1);
			}

			if(st.st_size >= min && st.st_size <= max_aux){
				informacion.inodo = st.st_ino;
				informacion.size = st.st_size;

				if(write(fd[1], &informacion, sizeof(informacion)) < 0){
					perror("Error al escribir - 1er hijo");
					exit(1);
				}
			}


		}	

		closedir(directorio);
		exit(0); //PARA QUE NO SIGA
	}

	// SEGUNDO HIJO
	if((pid = fork()) < 0){
		perror("Error al crear el hijo");
		exit(1);
	}else if(pid == 0){ // HIJO 1
		close(fd[0]); // Cerramos CAUCE LECTURA
		min_aux = mitad + 1;

		if((directorio = opendir(".")) == NULL){
			perror("Error al abrir directorio - 2do hijo");
			exit(1);
		}

		while((entrada = readdir(directorio)) != NULL){
			if(stat(entrada->d_name,&st) < 0){
				perror("Error al hacer el stat - 2do hijo");
				exit(1);
			}

			if(st.st_size >= min_aux && st.st_size <= max){
				informacion.inodo = st.st_ino;
				informacion.size = st.st_size;

				if(write(fd[1], &informacion, sizeof(informacion)) < 0){
					perror("Error al escribir - 1er hijo");
					exit(1);
				}
			}


		}	

		closedir(directorio);
		exit(0); //PARA QUE NO SIGA
	}

	// CODIGO PADRE
	close(fd[1]); // Cierra CAUCE ESCRITURA
	//printf("inodo: %lu\n", inodo);

	// El padre lee cada uno de los inodos enviados por el hijo
	// Tras esto, lee el tamanio del inodo
	while(read(fd[0],&inodo,sizeof(inodo)) > 0){
		if(read(fd[0], &size, sizeof(size)) < 0){
			perror("Error al leer size");
			exit(1);
		}

		// Realizamos comprobaciones
		if((directorio = opendir(".")) == NULL){
			perror("Error al abrir en el directorio - Padre");
			exit(1);
		}

		int encontrado = 0;

		while((entrada = readdir(directorio)) != NULL && (encontrado == 0)){
			// Encuentra el inodo que le ha pasado un hijo
			if(entrada->d_ino == inodo){
				encontrado = 1;
				if(stat(entrada->d_name,&st) < 0){
					perror("Error al hacer el stat - PADRE");
					exit(1);
				}

				if(st.st_size == size){
					printf("TODO CORRECTO :)\n");
				}else{
					// No tiene el tamanio enviado :(
					printf("ERRRRROOOOOR!!:(\n");
				}
			}
		}

		if(encontrado == 0){
			printf("NO SE ENCONTRO EL INODO");
		}
		closedir(directorio);
	}
}