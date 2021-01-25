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
		exit(-1);
	}
}

int main(int argc, char **argv){

	int fdbloqueo, leidos;
	char buffer[tamano], fifoproxy[tamano];
	FILE *tmp;

	// Creación del archivo temporal
	if((tmp = tmpfile()) < 0){
		perror("Proxy: error al crear el archivo temporal");
		exit(1);
	}

	// Apertura del archivo de bloqueo
	if((fdbloqueo = open("archivo_bloqueo", O_RDWR|O_CREAT)) < 0){
		perror("\nProxy: problema con el archivo de bloqueo");
		exit(1);
	}

	while((leidos = read(STDIN_FILENO, buffer, tamano)) > 0){
		if(fwrite(buffer, sizeof(char), leidos, tmp) < 0){
			perror("\nProxy: error al escribir en el archivo temporal");
			exit(1);
		}
	}

	bloqueodesbloqueo(fdbloqueo, F_WRLCK);

	fseek(tmp, 0, SEEK_SET);

	while((leidos = fread(buffer, sizeof(char), tamano, tmp)) > 0){
		if(write(STDOUT_FILENO, buffer, leidos) < 0){
			perror("\nProxy: error al escribir en pantalla");
			exit(1);
		}
	}

	bloqueodesbloqueo(fdbloqueo, F_UNLCK);

	sprintf(fifoproxy, "fifo.%d", getpid());
	unlink(fifoproxy);

	exit(0);
}

