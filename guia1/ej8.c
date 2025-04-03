#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <string.h>	  // Corrected header
#include <sys/wait.h> // Added header for wait()

int main(int argc, char const *argv[])
{
	int dato = 0;
	pid_t pid = fork();
	// si no hay error, pid vale 0 para el hijo
	// y el valor del process id del hijo para el padre
	if (pid == -1)
		exit(EXIT_FAILURE);
	// si es -1, hubo un error
	else if (pid == 0) // el hijo
	{
		for (int i = 0; i < 3; i++)
		{
			dato++;
			printf("Dato hijo: %d\n", dato);
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			printf("Dato padre: %d\n", dato);
		}
	}
	exit(EXIT_SUCCESS); // cada uno finaliza su proceso
} 
// ¿Son iguales los resultados mostrados de la variable dato para el padre y para el hijo? ¿Qué está
// sucediendo? No ya que los proocesos hijos no comparten memoria con el padre.