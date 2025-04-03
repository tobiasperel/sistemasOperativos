#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pid_padre;
pid_t pid_hijo;
int contador = 0;
int pipe_fd[2];
enum {READ, WRITE};

void handlePing(int sig) {
    printf("ping (PID: %d)\n", pid_hijo);
    kill(pid_padre, SIGUSR2);  // El hijo avisa al padre
}

void handlePong(int sig) {
    printf("pong (PID: %d)\n", pid_padre);
    contador++;
    if (contador < 3) {
        kill(pid_hijo, SIGUSR1);  // El padre le avisa al hijo
    } else {
		//pregunto si quiero terminar el juego
		printf("¿Quieres terminar el juego? (s/n): ");
		char respuesta;
		scanf(" %c", &respuesta);
		if (respuesta == 's' || respuesta == 'S') {
			printf("Fin del juego\n");
			kill(pid_hijo, SIGTERM);  // Termina al hijo
			wait(NULL);
			exit(EXIT_SUCCESS);
		}
		contador = 0;  // Reinicia el contador
		kill(pid_hijo, SIGUSR1);  // El padre le avisa al hijo
    }
}

void hijo() {
    pid_hijo = getpid();
    write(pipe_fd[WRITE], &pid_hijo, sizeof(pid_hijo));
    close(pipe_fd[READ]);  // El hijo no lee
    close(pipe_fd[WRITE]); // El hijo no escribeS

    signal(SIGUSR1, handlePing);

    while (1) {
        pause();  // Espera señales
    }
}

int main() {
    pipe(pipe_fd);
    pid_padre = getpid();

    signal(SIGUSR2, handlePong);

    pid_hijo = fork();
    if (pid_hijo == 0) {
        hijo();
    }

    // Padre
    close(pipe_fd[WRITE]);  // Solo lee del pipe
    read(pipe_fd[READ], &pid_hijo, sizeof(pid_hijo)); // aca hace un wait si no hay nada
    close(pipe_fd[READ]);

    // Comienza el juego
    kill(pid_hijo, SIGUSR1);

    while (1) {
        pause();  // Espera señales del hijo
    }

    return 0;
}
