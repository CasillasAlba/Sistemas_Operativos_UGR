//Programa que muestre por pantalla el nombre de todos los ficheros regulares contenidos en el directorio
//pasado por argumento (Para ejecutar probar con el directorio padre: ./ejer .)

/*
    Para probarlo:
    mkdir dira
    touch dira/{a.txt,b.txt}
    mkdir dira/dirb

    FALLARIA Porque al buscar un fichero de otro directorio distinto del actual
    Hay que indicar la ruta!!

*/

#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    
    if( argc != 2){
        fprintf (stderr, "Error. Numero de parametros incorrecto");
        exit(2);
    }

    DIR * directorio;
    struct dirent * entrada;
    char ruta[512]; //El tamanio no es relevante (pero que sea grande grande)

    struct stat st;

    if((directorio = opendir(argv[1])) == NULL){
        perror("Error al abrir el directorio");
        exit(1);
    }

    while((entrada = readdir(directorio)) != NULL){

        /*

        ESTO FUNCIONA SOLO CON EL DIRECTORIO ACTUAL PORQUE PARA
        OTROS DIRECTORIOS HAY QUE INDICAR LA RUTA

        if(stat(entrada->d_name, &st) < 0){
            perror("Error en el stat");
            exit(1);
        }

        //Hay que hacer un stat por cada iteracion del while (es decir, por cada entrada del directorio)
        stat(entrada->d_name, &st);

        if(S_ISREG(st.st_mode)){   

            printf("\n Nombre: %s\n", entrada->d_name);
            
        }

        */

        //Mostrar el directorio y el nombre de la entrada
        //sprintf : en vez de guardarlo por pantalla, lo guarda en una cadena de caracteres pasada como primer argumentos

        sprintf(ruta,"%s/%s", argv[1], entrada->d_name);

        //Realizamos el stat de LA RUTA
        
        if(stat(ruta, &st) < 0){
            perror("Error en el stat");
            exit(1);
        }

        if(S_ISREG(st.st_mode)){   

            printf("\n Nombre: %s\n", entrada->d_name);
            
        }

    }

    closedir(directorio);
}