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
	int fd;
	pid_t pid;

	if(mkfifo("MI_FIFO", S_IRWXU) < 0){
		perror("Error al abrir el fifo");
		exit(1);
	}

	// LA IDEA ES QUE AMBOS HIJOS SE COMUNIQUEN POR EL CAUCE

	if((pid = fork()) < 0){
		perror("Error al crear el hijo");
		exit(1);
	}else if(pid == 0){
		if((fd = open("MI_FIFO", O_WRONLY)) < 0){
			perror("Error al abrir el fifo");
			exit(1);
		}

		if(dup2(fd, STDOUT_FILENO) < 0){
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
		if((fd = open("MI_FIFO", O_RDONLY)) < 0){
			perror("Error al abrir el fifo");
			exit(1);
		}
		

		if(dup2(fd, STDIN_FILENO) < 0){
			perror("Error en el dup2");
			exit(1);
		}

		execlp("cut", "cut", "-d:", "-f6", NULL);
		perror("Error en el exec");
		exit(1);
	}

	// Espera a los DOS HIJOS
	wait(NULL);
	wait(NULL);
}
