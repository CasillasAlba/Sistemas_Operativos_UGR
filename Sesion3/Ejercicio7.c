// Programa que cree 5 hijos. Cada hijo tiene que mostar su PID y el de su padre.

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(){
	pid_t pid;

	for(int i = 0; i < 5; i++){
		if((pid = fork()) < 0){
			perror("Error en el fork");
			exit(1);
		}else if(pid == 0){ // HIJO
			printf("Mi PID es: %d, y el PID de mi padre es: %d\n", getpid(), getppid());
			// PARA QUE LOS HIJOS DEL PROCESO NO CREEN HIJOS Y EVITAR BOMBA DE FORK!!
			exit(0); 
		}
	}

	for(int i = 0; i < 5; i++){
		wait(NULL);
	}

	printf("SOY EL PAPÁ %d\n", getpid());

}