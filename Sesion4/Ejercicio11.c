// Programa que cree dos hijos: el primero ejecuta greep root /etc/passwd
// El segundo ejecuta cut .d : -f6
// de forma que la salida debe de quedar: greep root /etc/passwd | cut .d : -f6
// El padre va a esperar a que ambos hijos terminen


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(){
	int fd[2];
	pid_t pid;

	if(pipe(fd) < 0){
		perror("Error al abrir el cauce");
		exit(1);
	}

	// LA IDEA ES QUE AMBOS HIJOS SE COMUNIQUEN POR EL CAUCE

	if((pid = fork()) < 0){
		perror("Error al crear el hijo");
		exit(1);
	}else if(pid == 0){
		close(fd[0]); // Cerramos CAUCE LECTURA

		/*
			grep escribe por pantalla, asi que con dup2
			hacemos que lo escriba en el cauce
		*/
		if(dup2(fd[1], STDOUT_FILENO) < 0){
			perror("Error en el dup2");
			exit(1);
		}

		execlp("grep", "grep", "root", "/etc/passwd", NULL);
		perror("Error en el exec");
		exit(1);
	}


	if((pid = fork()) < 0){
		perror("Error al crear el hijo");
		exit(1);
	}else if(pid == 0){
		// El segundo hijo LEE la primera parte de la orden
		close(fd[1]); // Cerramos CAUCE ESCRITURA

		if(dup2(fd[0], STDIN_FILENO) < 0){
			perror("Error en el dup2");
			exit(1);
		}

		execlp("cut", "cut", "-d:", "-f6", NULL);
		perror("Error en el exec");
		exit(1);
	}

	// EL padre tiene abierto los descriptores de fichero
	// Si no cierras, el cut se queda esperando (no finaliza)
	// a una posible escritura.
	// TODOS LOS PROCESOS CIERRAN EL PIPE AUNQUE NO LO USEN
	close(fd[0]);
	close(fd[1]);

	// Espera a los DOS HIJOS
	wait(NULL);
	wait(NULL);
}




