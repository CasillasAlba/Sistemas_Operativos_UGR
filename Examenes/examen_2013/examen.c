/*
	Construir un programa que cree un proceo hijo. Ambos utilizaran
	un cauce con/sin nombre para comunicarse. Donde cada uno hara:

	- Padre: lee el directorio actual y para cada archivo regular
	contenido en el mismo:
		1) Obtiene e imprime por pantalla el numero de inodo junto
		al UID del propietario
		2) Escribe el nombre del archivo en el cauce
	- Hijo: Lee el cauce el nombre del archivo escrito por el padre
	y muestra su contenido.

	AMPLIACION:
	1) El padre se asegura que los archivos cuyo nombre va a pasar al
	hijo tienen permiso de lectura para cada proceso, si no fuese asi,
	los cambia para que los pueda leer el hijo


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
	int fd, fd_fichero, leidos;
	pid_t pid;
	char buffer[512];
	DIR * directorio;
	struct dirent * entrada;
	struct stat st;
	char nombre_archivo[sizeof(entrada->d_name)];
	mode_t permisos;


	if(mkfifo("MI_FIFO", S_IRWXU) < 0){
		perror("Error al abrir el fifo");
		exit(1);
	}

	if((pid = fork()) < 0){
		perror("Error al crear un hijo");
		exit(1);
	}else if(pid == 0){ // HIJO
		if((fd = open("MI_FIFO", O_RDONLY)) < 0){ // El hijo es quien lee!
			perror("Error al abrir el fifo - HIJO");
			exit(1);
		}

		// El padre envia el nombre de varios archivos, por eso 
		// la lectura se hara en un while
		while(read(fd,nombre_archivo, sizeof(nombre_archivo)) > 0){
			printf("SE MUESTRA EL CONTENIDO DEL FICHERO %s\n", nombre_archivo);
			if((fd_fichero = open(nombre_archivo, O_RDONLY)) < 0){
				perror("Error al abrir el fichero enviado por el padre");
				exit(1);
			}
 
			while((leidos = read(fd_fichero,buffer,sizeof(buffer))) > 0){
				if(write(STDOUT_FILENO, buffer, sizeof(buffer)) < 0){
					perror("Error al escribir en la salida");
					exit(1);
				}
			}

			close(fd_fichero);
		}

		close(fd);
		exit(0); // Evitamos que el hijo continue
	}

	// Codigo padre
	if((fd = open("MI_FIFO", O_WRONLY)) < 0){ // El hijo es quien lee!
		perror("Error al abrir el fifo - HIJO");
		exit(1);
	}

	if((directorio = opendir(".")) == NULL){
		perror("Error al abrir el directorio");
		exit(1);
	}

	while((entrada = readdir(directorio)) != NULL ){
		if(stat(entrada->d_name, &st) < 0){
			perror("Error en el stat");
			exit(1);
		}

		if(S_ISREG(st.st_mode)){
			printf("inodo: %lu - UID: %d\n", st.st_ino, st.st_uid);
			
			// Ampliacion 1
			if((st.st_mode & S_IRUSR) != S_IRUSR){ // No tiene el permiso
				permisos = st.st_mode | S_IRUSR;
			}

			if(chmod(entrada->d_name, permisos) < 0){
				perror("Error al establecer los permisos");
				exit(1);
			}
			

			if(write(fd,entrada->d_name, sizeof(entrada->d_name)) < 0){
				perror("Error en el write");
				exit(1);
			}
		}
	}

	closedir(directorio);

}