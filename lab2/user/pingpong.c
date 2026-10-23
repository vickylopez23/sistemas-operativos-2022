#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void ping(int sem_ping, int sem_pong, int n)
{
    for (int i = 0; i < n; i++) // bucle de n interaciones
    {
        sem_down(sem_ping); // se decrementa el semaforo ping
        printf("ping\n"); // imprime ping
        sem_up(sem_pong); // se incrementa el semaforo pong
    }
}

static void pong(int sem_ping, int sem_pong, int n)
{
    for (int i = 0; i < n; i++) 
    {
        sem_down(sem_pong); // se decrementa el semaforo pong
        printf("    pong\n");
        sem_up(sem_ping); // se incrementa el semaforo ping
    }
}

int main(int argc, char **argv)
{
    int n = atoi(argv[1]); // toma el argumento que se pasa por consola
    if(n == 0){ // si es 0
        printf("Error en el argumento.\n"); //error
    }

    if (argc <= 1 || argc > 2) //verifico que se pase un  solo argumento
    {
        printf("ERROR in argc.\n");
        exit(0); //termina
    }

    int sem_ping = 0; // le asigno a ping a 0
    int sem_pong = 1; //le asigno a pong 1
    sem_open(sem_ping, 1); //inicio los  semaforos
    sem_open(sem_pong, 0);

    int pid = fork(); //gerenero un proceso hijo

    if (pid > 0) //estamos en el proceso padre
    {
        ping(sem_ping, sem_pong, n); //imprimo ping y setea el semaforo pong 
        wait(0);  //espera al proceso hijo
    }
    else if (pid == 0) //si estoy en el proceso hijo
    {

        pong(sem_ping, sem_pong, n); //imprimo pong y seteo el semaforo ping
    }
    else //si la llamada a fork no se concreto
    {
        printf("Error haciendo fork!"); //error
        return 1;
    }

    sem_close(sem_ping); //cierro los semaforos 
    sem_close(sem_pong);
    return 0;
}