/*
	Construya un programa llamado conSoporte.c. Cuando se ejecute
	desde el shell bash, obtendremos con getppid() el pid de dicha
	ejecucion del interprete de ordenes padre de conSoporte.
	Formamos la ruta del archivo que contiene el mapa de memoria
	de dicho proceso padre:

	char mempadre[60];
	sprintf(mempadre, "/proc/%d/maps", getppid());

	Este programa debera lanzar un proceso y se comunicaran con un
	cauce.

	- Hijo -> llama a exec para lanzar una ejecucion de la orden cat
	que obtenga el contenido del archivo /proc/<pid-del-padre>/maps

	- Padre -> leera caracter a caracter del cauce y procesara esta 
	secuencia de forma que cuando el caracter leido sea "/" es porque
	comienza la ruta de un archivo aludido en el archivo maps, y
	debera llevar a la salida estandar el resto de la linea (hasta
	encontrar un "\n").
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

int main(){
	int fd;
	pid_t pid;
	char mempadre[60];
	char c[1];
	int debo_escribir = 0;

	if(mkfifo("MEMPADRE", S_IRUSR | S_IWUSR) < 0){
		perror("Error al crear el fifo");
		exit(1);
	}

	sprintf(mempadre, "/proc/%d/maps", getppid());

	if((pid = fork()) < 0){
		perror("Error al crear el hijo");
		exit(1);
	}else if(pid == 0){ // HIJO
		if((fd = open("MEMPADRE", O_WRONLY)) < 0){
			perror("Error al abrir el cauce");
			exit(1);
		}

		if(dup2(fd, STDOUT_FILENO) < 0){ // Escribe en CAUCE
			perror("Error en dup2");
			exit(1);
		}

		execlp("cat", "cat", mempadre, NULL);
		perror("Error en el exec");
		exit(1);
	}

	if((fd = open("MEMPADRE", O_RDONLY)) < 0){
		perror("Error al abrir el cauce");
		exit(1);
	}

	while(read(fd, &c, sizeof(char)) > 0){
		if(strcmp(c,"/") == 0){
			debo_escribir = 1;
		}

		if(debo_escribir){
			// Escribir el caracter que acabamos de leer
			if(write(STDOUT_FILENO, &c, sizeof(char)) < 0){
				perror("Error al escribir");
				exit(1);
			}
		}
		if(strcmp(c,"\n") == 0){
			debo_escribir = 0;
		}
	}
}