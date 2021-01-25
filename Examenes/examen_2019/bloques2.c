/*
	Programa llamado bloques2.c que es una amplicacion del anterior.
	Para cada dato que lea el padre del cauce, el proceso padre guardara
	en un archivo /tmp y cn nombre <<dato_N.txt>> (donde N es un contador 
	iniciado en 1 y que por cada letra se va incrementando en una unidad),
	como contenido el valor de ruta exec sobre el que se ha caculado y el
	numero de bloques asociado.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char * argv[]){
	if(argc < 2){
		fprintf(stderr, "Debe de pasar al menos un argumento\n");
		exit(2);
	}

	int fd[2];
	pid_t pid;

	if(pipe(fd) < 0){
		perror("Error en el pipe");
		exit(1);
	}

	for(int i = 1; i < argc; ++i){
		//printf("%s\n",*(argv+i));
		if((pid = fork()) < 0){
			perror("Error al hacer el fork");
			exit(1);
		}
		 
		// HIJO -> ejecuta "du -k argv[i]"
		if(pid == 0){
			close(fd[0]); // Cierra CAUCE LECTURA
			dup2(fd[1], STDOUT_FILENO); // Escribe CAUCE
			execlp("du", "arg0", "-k",*(argv+i),NULL);
		}
	}

	// PADRE
	close(fd[1]); // Cierra CAUCE ESRITURA
	dup2(fd[0], STDIN_FILENO); // Lee cauce

	setbuf(stdout, NULL); // Limpiamos el buffer

	char buffer[1];
	int contador = 0;

	// Buffer que almacena el contenido evitando basura
	/*
		void *malloc(size_t size);
		Asigna la memoria solicitada y le devuelve un puntero

	*/
	char *bufferConcatenado = malloc(sizeof(char*));
	/*
		void *memset(void *str, int c, size_t n);
		Copia el caracter c a los primeros n caraceres
		de la cadena (es decir, rellena buffer de 0s)

	*/
	bufferConcatenado = memset(bufferConcatenado, 0, sizeof(char*));

	// Lee de STDIN_FILENO un caracter (uno por uno)
	// Si es un caracter -> lo concatena; y si se acaba (salto de linea)
	// lo escribimos en el archivo que hay que crear
	while((read(STDIN_FILENO, buffer, 1)) > 0){
		if(!strcmp(buffer, "\n")){ // Si leemos salto de linea creamos el archivo
			char * ruta = malloc(sizeof(char*)); // Ruta del archivo dato_N
			sprintf(ruta, "/tmp/dato_%d.txt", contador);

			int descriptor_fichero;
			if((descriptor_fichero = open(ruta, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU)) < 0){
				perror("Error al abrir descriptor_fichero");
				exit(1);
			}

			sprintf(bufferConcatenado, "%s\n", bufferConcatenado); // Aniadimos espacio al final

			if(write(descriptor_fichero, bufferConcatenado, strlen(bufferConcatenado)*sizeof(char*)) < 0){
				perror("Error al hacer el write");
				exit(1);
			}

			close(descriptor_fichero);

			printf("%s -> %s", ruta, bufferConcatenado);
			bufferConcatenado = memset(bufferConcatenado, 0, sizeof(char*)); // Limpiamos buffer
			contador++;
		}else{
			// Si leemos un caracter concatenamos
			sprintf(bufferConcatenado, "%s%s", bufferConcatenado, buffer); 
		}
	}

	int status;
	while((wait(&status)) > 0); // Evitamos procesos zombie

}