/*
	Construir un programa cliente.c que recibe dos argumentos:
	el numero de clientes y un directorio.
	Se calculan dos operandos: el primero sera el numero de inodo 
	del directorio y el segundo el tamanio del primer archivo regular 
	que se encuentre en ese directorio.
	Por cada cliente, mediante un numero aleatorio, se elegira la operacion 
	a realizar: 0 sera una suma '+' y 1 sera una multiplicacion '*'.

	Crear un programa calculadora.c que esperara las peticiones de los
	clientes y relaizara el calculo. Escribira lo resultados de vuelta para
	que clientes.c los imprima.

	Una vez resueltas las operaciones, tambien guardara los resultados en un
	archivo llamado historico.txt.

	Se controlara mediante un manejador que no queden hijos zombies
*/

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

#define longnombre 50

// Manejador para que no queden hijos zombies
void manejador(int s){
	wait(NULL);
}


// Funcion que genera el caracter de la operacion a realizar
void generaOperacion(char *op, pid_t pid_proceso){
	int aux;
	
	// Hacemos esto en vez de srand(time(NULL)) porque de lo contrario
	// la semilla solo seria aleatoria al principio. Proporcionando el
	// pid, cada cliente genera una semilla diferente.
	srand(pid_proceso);
	aux = rand()%2;

	if(aux == 0){
	 *op = '+';
	}else
	 *op = '*';	
}


// EL orden importa!!
struct datos{
	long int operando_a;
	long int operando_b;
	char op;
	pid_t pid_cli;
};


int main(int argc, char * argv[]){

	//Comprobacion de errores
  	if(argc != 3){
      	fprintf(stderr, "Error: numero de argumentos incorrectos");
      	exit(2);
  	}
	

  	// Variables para el manejador
	struct sigaction nuevo_valor;
    nuevo_valor.sa_handler = manejador;
    nuevo_valor.sa_flags = 0;


    if (sigaction(SIGCHLD, &nuevo_valor, NULL) < 0)
    {
        perror("sigaction");
        exit(1);
    }

    // Variables
	int num_clientes = atoi(argv[1]); 
	struct datos data;
	struct stat st;
	pid_t pid;
	pid_t hijos[num_clientes];
	int fd;
	DIR * directorio;
	struct dirent * entrada;
	char fifoEntrada[longnombre]; // Nombre de FIFO de entrada
	char fifoSalida[longnombre];
	int dfifoe, dfifos;
	// IMPORTANTE QUE SEA LONG INT PORQUE SI NO PRODUCE DESBORDAMIENTO
	long int resultado; 

	// Calculamos los operandos
	if((directorio = opendir(argv[2])) == NULL){
	    perror("Error al abrir el directorio");
	    exit(1);
	}

	if((entrada = readdir(directorio)) == NULL){
		perror("error en el readdir");
		exit(1);
	}


	if(stat(entrada->d_name, &st) < 0){
        perror("Error en el stat");
        exit(1);
   }

	// El primer operando es el numero de inodo del directorio
	data.operando_a = entrada->d_ino;


	//Segundo operando: el tamaño del primer archivo regular que se encuentre en ese directorio
	
	while((entrada = readdir(directorio)) != NULL){

	   if(stat(entrada->d_name, &st) < 0){
            perror("Error en el stat");
            exit(1);
        }

        if(S_ISREG(st.st_mode)){   
            data.operando_b = st.st_size;
            break; // Solo queremos el primero    
        }
    }


	for(int i= 0; i < num_clientes; i++){
	    if((pid = fork()) < 0){
	        perror("Error en el fork");
	        exit(1);
	    }else if(pid == 0){ // CLIENTE
	    	generaOperacion(&data.op, getpid());

	    	data.pid_cli = getpid();

			printf("Cliente(pid) %d genera operacion: %ld%c%ld\n", data.pid_cli, data.operando_a, data.op, data.operando_b);

			// Abrimos el fifo de entrada para escribir en el struct
			if ((dfifoe = open("MIFIFO_E", O_WRONLY)) < 0){
		        perror("Error al abrir el fifo de entrada");
		        exit(1);
		    }

		    // Abrimos el fifo de salida leer el resultado  	
		    if ((dfifos = open("MIFIFO_S", O_RDONLY)) < 0){
		      	perror("Error al abrir en fifo de salida");
		      	exit(1);
		    }

			if(write(dfifoe,&data,sizeof(data)) < 0){
				perror("Error al escribir en el cauce");
				exit(1);
			}

			// Recogemos la respuesta de calculadora.c
			if(read(dfifos,&resultado,sizeof(long int)) < 0){
				perror("Error al leer del cauce");
				exit(1);
			}

			printf("El resultado del cliente %d es: %ld\n", data.pid_cli, resultado);

			exit(0); //PARA QUE LOS HIJOS DEL PROCESO NO CREEN HIJOS -- EVITAMOS BOMBA DE FORK!!!
		}else{
			hijos[i] = pid;
		}
 
    }


    closedir(directorio);

	//Esperamos a que todos los clientes terminen
    int estado;
    pid_t espera_pid;
	for(int i = 0; i < num_clientes; i++){
    	espera_pid = waitpid(hijos[i], &estado, 0);
		printf("Cliente: Espera del hijo %i\n", espera_pid);
    }

	/*
	int estado;
	while(wait(&estado) != -1);
	exit(0);
	*/

}

