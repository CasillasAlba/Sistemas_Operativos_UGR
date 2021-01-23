// Programa que cree un hijo el cual va a buscar la primera entrada del directorio
// actual que sea un fichero regular y va a enviar el inodo de ese fichero por un cauce.
// El padre cuando lea el inodo mostrara por pantalla el numero de inodo y su tamanio en Bytes

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

int main(){
	int fd[2];
	pid_t pid;
	DIR * directorio;
	struct dirent * entrada;
	struct stat st;
	ino_t inodo;
	off_t size;

	if(pipe(fd) < 0){
		perror("Error al crear el pipe");
		exit(1);
	}

	if((pid = fork()) < 0){
		perror("Error al crear el fork");
		exit(1);
	}else if(pid == 0){
		// Cerramos cauce de lectura
		close(fd[0]);

		if((directorio = opendir(".")) == NULL){
			perror("error al abrir el directorio");
			exit(1);
		}

		while((entrada = readdir(directorio)) != NULL){
			if(stat(entrada->d_name,&st) < 0){
				perror("Error al hacer el stat");
				exit(1);
			}

			if(S_ISREG(st.st_mode)){
				if(write(fd[1], &st.st_ino, sizeof(st.st_ino)) < 0){
					perror("Error al escribir en el pipe");
					exit(1);
				}

				break; // Nos quedamos con el primero que encuentre
			}
		}
		closedir(directorio);
		exit(0);
	}else{
		// Cerramos cauce de escritura
		close(fd[1]);

		if(read(fd[0], &inodo, sizeof(inodo)) < 0){
			perror("Error al leer del pipe");
			exit(1);
		}


		if((directorio = opendir(".")) == NULL){
			perror("error al abrir el directorio");
			exit(1);
		}

		while((entrada = readdir(directorio)) != NULL){
			if(entrada->d_ino == inodo){
				if(stat(entrada->d_name,&st) < 0){
					perror("Error al hacer el stat");
					exit(1);
				}

				size = st.st_size;
				break;
			}
		}

		printf("inodo: %lu- tamanio: %ld\n", inodo, size);
		closedir(directorio);
	}
}


