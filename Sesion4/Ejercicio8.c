// Programa que cree un hijo, el cual va a leer un entero por teclado y se lo enviara
// al padre mediante un cauce, el cual leera y mostrara por pantalla

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(){
	pid_t pid;
	int fd[2];
	int valor;

	if(pipe(fd) < 0){
		perror("Error en la creacion de pipe");
		exit(1);
	}

	if((pid = fork()) < 0){
		perror("Error al hacer el fork");
		exit(1);
	}else if(pid == 0){ // HIJO
		// Como el hijo va a escribir por el cauce, cerramos el de lectura
		close(fd[0]);

		// leer entero por teclado
		scanf("%d", &valor);

		if (write(fd[1], &valor, sizeof(int)) < 0){
			perror("Error al enviar el valor");
			exit(1);
		}

	}else{ // PADRE
		// Como el padre va a escribir, cerramos el cauce de escritura
		close(fd[1]);

		if (read(fd[0], &valor, sizeof(int)) < 0){
			perror("Error al recibir el valor");
			exit(1);
		}

		printf("Leido: %d\n", valor);

	}
}