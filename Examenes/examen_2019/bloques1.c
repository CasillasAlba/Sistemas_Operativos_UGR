/*
	La orden: du -k rutacompletaarchivo proporciona el numero de bloques
	(de 1024 BYTES) asignados a rutacompletaarchivo.

	Construir un programa bloques1.c que recibira cierto numero de argumentos
	cada uno de ellos es la ruta de un archivo. Para cada argumento recibido
	(i-esimo), debera crearse un hijo que lance la ejecucion de la orden du 
	pasandole como primer argumento <<-k>> y como segundo argumento argv[i].

	Tenemos entonces al proceso padre y a argc ejecutandose concurrentemente.
	Debera programarse las redirecciones oportunas para que se comuniquen
	adecuadamente a traves de un unico cauce donde cada hijo debera llevar su
	salida y donde el padre debe leer. El padre va leyendo cada dato y lo muestra
	en la salida estandar. Debe conseguir la mayor concurrencia posible.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

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

	// Imprimimos lo que han escrito los hijos
	char buffer[100];
	while((read(STDIN_FILENO,buffer,sizeof(buffer))) > 0){
		printf("%s",buffer);
	}

	int status;
	while((wait(&status)) > 0); //Evitamos procesos zombie
}