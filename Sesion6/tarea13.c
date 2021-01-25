/*
	Se le aplica un cerrojo a un archivo. Si el bloqueo falla, el programa
	escaena el archivo para mostrar la informacion sobre los cerrojos 
	existentes: el nombre del archivo, el PID del proceso, el inicio y longitud de
	la region bloqueada, y si es exclusivo (w) o comartido(r).
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 

int main(int argc, char * argv[]){
	const char ARCHIVO[] = "archivo.txt";
	struct flock cerrojo;
	int fd;

	// Abrimos el archivo
	fd = open(ARCHIVO, O_RDWR);
	if(fd < 0){
		perror("Open error");
		exit(1);
	} 

	// Configuramos el cerrojo
	cerrojo.l_type = F_WRLCK;
	cerrojo.l_whence=SEEK_SET; // Otras opciones: SEEK_CUR o SEEK_END
	// Que l_stat y l_len valgan 0, significa que bloqueas el archivo ENTERO
	cerrojo.l_stat =0; // Desde donde lo bloqueo
	cerrojo.l_len =0;  // Longitud del bloqueo
	cerrojo.l_pid =0;

	// Lo bloqueamos
	if(fcntl(fd, F_SETFLK, &cerrojo) == -1){
		// Ha habido un error, entonces obtenemos los datos del cerrojo
		fcntl(fd, F_GETLK, &cerrojo);
		printf("Archivo bloqueado\n");
		printf("\t- Archivo: %s\n\t - PID: %i\n\t - Tipo:
			%c\n", ARCHIVO, cerrojo.l_pid, cerrojo.l_type==F_WRLCK ? 'w': 'r');
	}else{
		// Bloqueado con exito
		printf("El archivo ha sido bloqueado con exito\n");
		sleep(5);
	}

	// DESBLOQUEAMOS EL CERROJO
	cerrojo.l_type = F_UNLCK;
	cerrojo.l_whence=SEEK_SET; // Otras opciones: SEEK_CUR o SEEK_END
	cerrojo.l_stat =0;
	cerrojo.l_len =0;
	cerrojo.l_pid =0;
}

