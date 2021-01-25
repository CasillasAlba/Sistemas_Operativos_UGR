// EJEMPLO DE EJECUCION: ./servidor nombreFifo
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#define longnombre 50

void mimanejador(int senal){

	int estado;
	pid_t pid;

	pid = wait(&estado);
}

int main(int argc, char **argv){
	// Como son fifos conocidos, el nombre de los fifos
	// se lo pasamos como argumento
	if(argc < 2){
		fprintf(stderr, "SERVIDOR: Numero de parametros incorrecto\n");
		exit(2);
	}

	// Variables
	char nombrefifoe[longnombre]; //fifo de entrada
  	char nombrefifos[longnombre]; //fifo de salida
  	char nombrefifoproxy[longnombre]; // fifo del proxy
  	int dfifoe, dfifos, fdproxy;
  	int resultado; // Resultado del read
  	int pid, pidproxy;
  	int fdbloqueo;

  	// Montamos los nombres de los fifos
  	sprintf(nombrefifoe, "%se", argv[1]);
	sprintf(nombrefifos, "%ss", argv[1]);

	// Creamos los fifos
	if(mkfifo(nombrefifoe, S_IRWXU) == -1){
		if(errno == EEXIST){
			printf("SERVIDOR: El fifo %s de entrada ya esta creado\n", nombrefifoe);
		}else{
			perror("SERVIDOR: Error al crear el fifo de entrada");
		}

		exit(1);
	}

	if(mkfifo(nombrefifos, S_IRWXU) == -1){
		if(errno == EEXIST){
			printf("SERVIDOR: El fifo %s de salida ya esta creado\n", nombrefifos);
		}else{
			perror("SERVIDOR: Error al crear el fifo de salida");
		}

		exit(1);
	}

	// Apertura de los archivos FIFOs conocidos de entrada y salida
	if((dfifoe = open(nombrefifoe, O_RDWR)) < 0){
  		perror("\nSERVIDOR: problema con el archivo fifo de entrada al servidor");
  		exit(1);
	}
	if((dfifos = open(nombrefifos, O_RDWR)) < 0){
		perror("\nSERVIDOR: problema con el archivo fifo de salida al servidor");
		exit(1);
	}


	
	signal(SIGCHLD, mimanejador);
	// Creación y apertura del archivo de bloqueo
	if((fdbloqueo = open("archivo_bloqueo", O_CREAT, S_IRWXU)) < 0){
           perror("SERVIDOR: problema con el archivo de bloqueo");
           exit(1);
	}

	// 1) Leer la informacion proporcionada por el cliente
	// Hacemos un while porque esta todo el rato a la espera de las
	// peticiones de los clientes
	while((resultado = read(dfifoe, &pid, sizeof(int))) > 0){
		printf("SERVIDOR: El proceso cliente %d pide escribir\n", pid);
		// 2) Mandarle la informacion del PROXY

		// Necesitamos crear un proceso proxy que se encargue de ese cliente
		// COMO EL SERVIDOR ESTA SIEMPRE ESPERANDO PETICIONES DE CLIENTE
		// LA OPCION IDEAL SERIA LANZAR EL PROXY (PROGRAMA EXTRA) HACIENDO
		// UN HIJO QUE REALICE EL EXECTL, PORQUE SI LO HACE EL SERVIDOR
		// DIRECTAMENTE, DEJARIA DE ESPERAR MAS PETICIONES DE CLIENTES
		if(pid = fork() < 0){
			perror("\nSERVIDOR: error en el fork");
			exit(1);
		}
	
		if(pid == 0){ // hijo (ejecutara el proxy)
			// Obtener el pid del proceso (PORQUE LO PIDE EL CLIENTE)
			pidproxy = getpid();

			// CREAMOS EL FIFO QUE COMUNICA EL CLIENTE CON EL PROXY
			// Se llamara fifo.pid_proxy (ejemplo: fifo.1234)
			sprintf(nombrefifoproxy, "fifo.%d", pidproxy);
			
			// Crear el fifo
			if(mkfifo(nombrefifoproxy, S_IRWXU) == -1){
				if(errno == EEXIST){
					printf("SERVIDOR: El fifo %s del proxy ya esta creado\n", nombrefifoproxy);
				}else{
					perror("SERVIDOR: Error al crear el fifo del proxy");
				}

				exit(1);
			}
			
			// Escribimos el pid en el fifo
			// SI DA ERROR
			if(!(resultado = write(dfifos, &pidproxy, sizeof(int)))){
				perror("\nPROXY: error al escribir el proxy asignado");
				exit(1);
			}

			// SI NO DA EROR
			printf("PROXY: Escrito en el FIFO el PID%d\n", pidproxy);

	
			// Apertura del FIFO asignado para la comunicación con el proxy
			if((fdproxy = open(nombrefifoproxy, O_RDONLY)) < 0){
				perror("\nPROXY: problema con el archivo fifo del proxy");
				exit(1);
			}

			// Redireccionamos la entrada estandar (el archivo dfifoproxy) 
			dup2(fdproxy, STDIN_FILENO);
			close(fdproxy);

			// Ejecución del proxy
			if((execlp("./proxy", "proxy", NULL) < 0)){
				perror("\nServidor: error en execlp");
				exit(1);
			}
		}
	}

	unlink("archivo_bloqueo");
	unlink(nombrefifoe);
	unlink(nombrefifos);
	
	exit(0);
}

