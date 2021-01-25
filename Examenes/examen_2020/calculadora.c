#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <wait.h>
#include <string.h>

#define longnombre 50

//Para la exclusion mutua del ejercicio 3
void bloqueodesbloqueo(int dbloqueo, int orden){

	struct flock cerrojo;

	// Inicializamos el cerrojo para bloquear todo el archivo
	cerrojo.l_type = orden;
	cerrojo.l_whence = SEEK_SET;
	cerrojo.l_start = 0;
	cerrojo.l_len = 0;

	// Si vamos a bloquearlo y ya lo está, entonces el proceso duerme
	if(fcntl(dbloqueo, F_SETLKW, &cerrojo) < 0){
		perror("\nProxy: problemas al bloquear para impresion");
		exit(1);
	}
}

struct datos{
	long int operando_a;
	long int operando_b;
	char op;
	pid_t pid_cli;
};

int main(){
	int fdbloqueo, fdtxt; //Para la exclusion mutua del ejercicio 3 e historico datos
	struct datos data;
	// IMPORTANTE QUE SEA LONG INT PORQUE SI NO PRODUCE DESBORDAMIENTO
	long int resultado;
	int tamanio = 3;
	//pid_t *hijos = malloc(tamanio*sizeof(pid_t));
	pid_t hijos[tamanio];
	int dfifoe, dfifos;
	// Como vamos a concatenar las operaciones de los clientes, 
	// malloc devuelve un puntero con tamanio char, lo multiplicaremos
	// *1000 para que tenga espacio de sobra para almacenar todas las
	// operaciones. De esta forma, no se accedera a espacio reservado
	char *bufferHistorico = malloc(sizeof(char*)*2000);
	


	// CREAMOS LOS FIFO
	if(mkfifo("MIFIFO_E", S_IRWXU) < 0){
		if(errno == EEXIST)
			printf("Calculadora: El fifo de entrada ya esta creado\n");
		else
			perror("Calculadora: Error al crear el fifo de entrada\n");
		exit(1);
	}

	if(mkfifo("MIFIFO_S", S_IRWXU) < 0){
		if(errno == EEXIST)
			printf("Calculadora: El fifo de salida ya esta creado\n");
		else
			perror("Calculadora: Error al crear el fifo de salida\n");
		exit(1);
	}

  	// Abrimos el fifo de entrada para que calculadora lea el struct
	if ((dfifoe = open("MIFIFO_E", O_RDONLY)) < 0){
		perror("Error al abrir el fifo de entrada");
		exit(1);
	}

	// Abrimos el fifo de salida escribir el resultado  	
	if ((dfifos = open("MIFIFO_S", O_WRONLY)) < 0){
		perror("Error al abrir en fifo de salida");
		exit(1);
	}

	if((fdbloqueo = open("archivo_bloqueo", O_RDWR|O_CREAT)) < 0){
		perror("\nProxy: problema con el archivo de bloqueo");
		exit(1);
	}

	int estado;
    pid_t espera_pid;
    char * valor;
	
 	// Leemos lo que vayan pasandole los hijos
 	int contador = 0;
	while (read(dfifoe, &data, sizeof(data)) > 0){
		hijos[contador] = data.pid_cli;
		printf("CLIENTE %d , OPERACION: %ld%c%ld\n", data.pid_cli,data.operando_a, data.op, data.operando_b);
		printf("PID %d , CLIENTE %d ", data.pid_cli, hijos[contador]);

		if(data.op == '+' ){
			resultado = data.operando_a + data.operando_b;
		}else if(data.op == '*'){
			resultado = data.operando_a * data.operando_b;
		}

		printf("El resultado de la operacion es: %ld\n", resultado);
		// Guardo la operacion en un char* para almacenar la informacion en historico.txt!!
		sprintf(bufferHistorico, "%s\n%ld%c%ld=%ld", bufferHistorico, data.operando_a, data.op, data.operando_b, resultado);

		
		// Se escribe de vuelta para que el cliente lo escriba
		if(write(dfifos, &resultado, sizeof(long int)) < 0){
			perror("Error al escribir en el write");
		}

		contador++;
		//tamanio++;
		//hijos = realloc(hijos,tamanio);
		//scanf("%c", &valor);

    }
    
    
    for(int i = 0; i < tamanio; i++){
    	espera_pid = waitpid(hijos[i], &estado, 0);
		printf("Calculadora: Acaba de finalizar mi hijo %i\n", espera_pid);
    }
    
    

	//Una vez que se resuelva las operaciones, guardarla en el historico de archivos (EJERCICIO3)
	
    if((fdtxt = open("historico.txt", O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH )) < 0){
        perror("error en el open");
        exit(1);
    }

    //printf("Buffer: %s", bufferHistorico);
   
    if(write(fdtxt, bufferHistorico, strlen(bufferHistorico)*sizeof(char*)) < 0){ // no strlen sino un numero (inicio-final)
    	perror("Error al escribir en historico.txt");
    	exit(1);
    }
    

    close(fdtxt);
    bufferHistorico = memset(bufferHistorico, 0, sizeof(char*)); // Limpiamos el buffer
	bloqueodesbloqueo(fdbloqueo, F_UNLCK);

}



