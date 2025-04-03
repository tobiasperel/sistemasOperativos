#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <string.h> // Corrected header
#include <sys/wait.h> // Added header for wait()



void nieto(const char* nombre){ // Changed 'string' to 'const char*'
	printf("Soy %s, el nieto de Abraham\n", nombre);
	sleep(1);
	exit(EXIT_SUCCESS);
}

void hijo1(){
	pid_t pid_bart;
	pid_t pid_lisa;
	pid_t pid_maggie;

	pid_bart = fork();
	if(pid_bart == 0) nieto("Bart");

	pid_lisa = fork();
	if(pid_lisa == 0) nieto("Lisa");

	pid_maggie = fork();
	if(pid_maggie == 0) nieto("Maggie");

	// Esperar a que los nietos terminen
	wait(NULL);
	wait(NULL);
	wait(NULL);

	printf("Soy Homero, el padre de Bart, Lisa y Maggie\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
	pid_t pid_homero;
	printf("Soy Abraham, el proceso padre\n");

	pid_homero = fork();
	if(pid_homero == 0) hijo1();
	else{
		wait(NULL);
		printf("Soy Abraham ya estan todos muertos\n");
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_SUCCESS);
}