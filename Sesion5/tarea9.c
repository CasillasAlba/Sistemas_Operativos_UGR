// tarea9.c 
// FUNCIONAMIENTO:
// compilamos: gcc tarea9.c -o tarea9
// ejecutamos: ./tarea9
// SI EN LA TERMINAL HACEMOS CONTROL+C LA IGNORA
#include <stdio.h>
#include <signal.h>

int main()
{
	// 1) Creamos la estructura sigaction
	struct sigaction sa;
	// 2) Indicamos que su manejador (sa_handler) ignore la señal
	sa.sa_handler = SIG_IGN; // ignora la senial
	// 3) Si por si acaso la mascara que hemos creado tiene algo (la queremos vacía)
	// solo aniadimos las seniales QUE NOSOTROS LE DECIMOS inicianizandola a una mascara vacia
	sigemptyset(&sa.sa_mask); 
	
	// 4) Reinicializamos todas las banderas
	sa.sa_flags = SA_RESTART; 
	
	// 5) Asignamos a la interrupcion por teclado que ignore la senial
	if (sigaction(SIGINT, &sa, NULL) == -1){
		printf("error en el manejador");
	}
	while(1);
}
