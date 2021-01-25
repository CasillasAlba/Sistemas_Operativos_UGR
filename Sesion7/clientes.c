// EJEMPLO DE EJECUCION: ./clietes nombreFifo num_clientes 

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#define tamano 1024
#define longnombre 50

// Manejador de senal (SIGPIPE) para cuando se escribe en FIFO no abierto aun 
// para lectura, por defecto esta senal terminaria el proceso.
static void mimanejador(int senal){
  printf ("Cliente %d: escribo en el FIFO aun no abierto para lectura\n", getpid());
  return;
}


// Funcion que devuelve un descriptor al FIFO a utilizar para imprimir. 
// Escribe en el fifo del servidor su pid
int obtenerfifo (int dfifoe, int dfifos, int mipid){

	int mififo, resultado, pidproxy;
	char nombrefifoproxy[longnombre];
	
	// Escribe cualquier cosa en el FIFO conocido del servidor
	//ESCRIBE SU PID !!!
	if (! (resultado =write (dfifoe,&mipid,sizeof(int)))) {
	  perror ("Cliente: error al escribir peticion en fifo conocido del servidor");
	  exit(EXIT_FAILURE);
	}
	printf("Cliente %d: escrito pid en fifo conocido del servidor.\n",mipid);
	
	// Obtiene el pid del proceso proxy con el que se va a comunicar
	// SE QUEDA ESPERANDO EL PID DEL PROXY DEL FIFOS
	if (! (resultado = read (dfifos,&pidproxy,sizeof(int)))) {
	  perror ("Cliente: error al leer fifo proxy asignado por servidor");
	  exit(EXIT_FAILURE);
	}
	printf("Cliente %d: leido pid %d en fifo de servidor. \n",mipid,pidproxy);
	
	// Abre el FIFO a utilizar en la comunicacion y lo devuelve
	sprintf(nombrefifoproxy,"fifo.%d",pidproxy);
  // NO OLVIDAR QUE EN SERVIDOR.C TAMBIEN NECESITAMOS ABRIR EL FIFO!!!!
	if ((mififo = open (nombrefifoproxy, O_WRONLY)) == -1) {
	  perror ("Cliente: problema al abrir fifo proxy asignado");
	  exit(EXIT_FAILURE);
	}
	return (mififo);
}


// Funcion que se encarga de generar un caracter un numero de veces
// y escribirlo en el FIFO correspondiente
// Parametro: C - Caracter que se escribira en el archivo FIFO
// Parametro: mififo Archivo FIFO
// Parametro: mipid Identificador del proceso

void producir (char c, int mififo, int mipid) {

  char bufer[tamano];
  int contador=0, i=0, resultado;
  int numcar;

  // Genera un numero aleatorio entre 1 y 10000, va a ser el numero de caracteres a enviar.
  srand ((unsigned int) mipid);
  numcar = 1 + (int) (10000.0*rand()/(RAND_MAX+1.0));
  printf("Cliente %d: Numero caracteres %c a generar es %d\n",mipid,c,numcar);

  // Pone el manejador para la senal SIGPIPE.
  if (signal(SIGPIPE, mimanejador)== SIG_ERR)
    perror ("Cliente: Error configurando senal");

  // Escritura de los datos.
  // Va guardando letras en el buffer y llega un punto en el que escribe
  // en el fifo
  while (contador<numcar){
    bufer[i] = c;
    contador++;
    i++;
    if ((i == tamano) || (contador == numcar)) {
      sleep((mipid+contador)%1);
      //Escribe en el fifo
      if ((resultado = write(mififo, bufer, i)) == -1){
        perror ("ERROR AL ESCRIBIR EN EL FIFO");
        exit(1);
      }
  	 
      printf("Cliente %d: He escrito en el fifo %d\n",mipid,resultado);
      i= 0;
    }
  }
}

int main (int argc, char **argv) {  

  char c = 'a'; 
  int numcli; // Numero de clientes
  int pid, mipid; // Identificadores de los procesos (de los hijos y del propio, respectivamente)
  int i; // Contador
  int dfifoe, dfifos, mififo; // Descriptores: entrada, salida y particular de cada peticion
  int estado;
  char nombrefifoe[longnombre]; //fifo de entrada
  char nombrefifos[longnombre]; //fifo de salida

  setbuf(stdout, NULL);

  if (argc < 3) {
    printf ("Clientes: Formato: clientes <nombre_fifo> <numero_clientes>\n");
    exit(EXIT_FAILURE);
  }else if ( ((numcli= atoi (argv[2])) > 10) || (numcli < 1) ){
    printf ("Clientes: <numero_clientes> comprendido entre 1 y 10\n");
    exit(EXIT_FAILURE);
  }

  // Compone los nombres de los FIFOs conocidos a partir del parametro,
  // uno de entrada y otro de salida (desde el punto de vista del servidor).
  // Se crea las dos variables de los fifos
  sprintf(nombrefifoe,"%se",argv[1]);
  sprintf(nombrefifos,"%ss",argv[1]);

  //Abrimos los fifo
  if ((dfifoe = open (nombrefifoe, O_WRONLY)) == -1) {
    perror ("Clientes: Problema con el archivo fifo de entrada al servidor");
    exit(EXIT_FAILURE);
  }
  if ((dfifos = open (nombrefifos, O_RDWR)) == -1) {
    perror ("Clientes: Problema con el archivo fifo de salida del servidor");
    exit(EXIT_FAILURE);
  }

  // Crea procesos clientes, con un caracter distinto a imprimir cada uno.
  for (i=1; i<numcli; i++) {
    if ((pid = fork()) == -1) {
      perror ("Clientes: Problema al crear proceso cliente");
      exit(EXIT_FAILURE);
    }

    if (pid == 0 ) {
      mipid = getpid();
      mififo = obtenerfifo(dfifoe, dfifos, mipid); // Obtenemos el fifo propio del proceso
      producir(c, mififo, mipid); // Producimos su documento
      exit(EXIT_SUCCESS);
    }else
      c++;
  }

  // El propio programa se convierte en un cliente.
  mipid = getpid ();
  mififo = obtenerfifo(dfifoe, dfifos, mipid); // Obtenemos el fifo propio del proceso
  producir(c, mififo, mipid); // Producimos su documento

  // El proceso original elimina zombies ya que es el padre.
  while (wait(&estado)!=-1);
  exit(EXIT_SUCCESS);
}




