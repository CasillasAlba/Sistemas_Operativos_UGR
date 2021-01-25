//Cuando llegue la senial SIGCHILD, mostramos el pesame por la muerta del hijo (RIP)
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void manejador(int s)
{
    printf("RIP");
    wait(NULL);
}

int main()
{
    struct sigaction nuevo_valor;
    nuevo_valor.sa_handler = manejador;
    nuevo_valor.sa_flags = 0;

    setbuf(stdout, NULL);

    if (sigaction(SIGCHLD, &nuevo_valor, NULL) < 0)
    {
        perror("sigaction");
        exit(1);
    }

    while (1)
    {
        sleep(5);
        if (fork() == 0)
        {
            printf("Soy un hijo\n");
            sleep(1);
            printf("Finalizo\n");
            exit(0);
        }
    }
}
