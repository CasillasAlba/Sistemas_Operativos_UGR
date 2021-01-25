// tarea11.c

#include <stdio.h>
#include <signal.h>

int main(){
	// Creamos una mascara nueva
	sigset_t new_mask;
	
	// inicializar la nueva mascara de seniales 
	sigemptyset(&new_mask);
	
	// Anideiamos la senial SIGUSR1 y SIGINT para que LAS IGNORE
	// Si usasemos la senial STOP, el proceso pararia pero NO moririra
	// Es decir, deja de ejecutarse pero si en la terminal hacemos ps -e | egrep tarea11, aparecera en memoria
	sigaddset(&new_mask, SIGUSR1);
	sigaddset(&new_mask, SIGINT);
	sigaddset(&new_mask, SIGKILL); // Esta NO la ignora porque SIGKILL NO SE PUEDE MODIFICAR
	
	//esperar a cualquier senial excepto SIGUSR1 
	// o sea, espera una orden que NO esta en la mascara
	sigsuspend(&new_mask);

}
