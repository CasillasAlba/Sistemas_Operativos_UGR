// tarea10.c 

#include <stdio.h>
#include <signal.h>

// En este caso nosotros le damos funcionalidad al manejador
static int s_recibida=0;
// ESTA FUNCION DEBE DE TENER POR DEFECTO UN PARAMETRO (EL NUMERO DE LA SENIAL)
// ESTE NUMERO LO COGE POR DEFECTO AL HACER EL SIGACTION
static void handler (int signum){
	printf("\n Nueva accion del manejador \n");
	s_recibida++;
}

int main(){
	struct sigaction sa;
	sa.sa_handler = handler; // ignora la senial
	sigemptyset(&sa.sa_mask); 
	
	//Reiniciar las funciones que hayan sido interrumpidas por un manejador 
	sa.sa_flags = SA_RESTART; 
	
	// Si llega la senial Control+C se auemnta en 1 el contrador s_recibida
	if (sigaction(SIGINT, &sa, NULL) == -1){ 
		printf("error en el manejador");
	}
	
	while(s_recibida<3);
}
