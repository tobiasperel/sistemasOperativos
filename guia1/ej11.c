#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int pipe_fd[2][2]; // 0 es para leer, 1 es para escribir son dos pipes(uno para cada hijo)
enum {READ, WRITE};
enum {PADREPIPE, HIJOPIPE};

void hijo() {
    int contadorHijo = 1;
    while (1){
        breceive(PADREPIPE); // Espero la señal del padre
        bsend(PADREPIPE, 1); // Envio la señal al padre
        contadorHijo+=2;
        printf("Soy el hijo, contadorHijo: %d\n", contadorHijo);
    }
    
    exit(0);
}

void bsend(int pipe_id, int msg) {
    write(pipe_fd[pipe_id][WRITE], &msg, sizeof(msg));
}

void breceive(int pipe_id) {
    int value;
    read(pipe_fd[pipe_id][READ], &value, sizeof(int));
}

// Manejadores de señales
void handle_sigusr1(int sig) {
    // El padre envía un mensaje al hijo
    bsend(HIJOPIPE, 1);
}

void handle_sigusr2(int sig) {
    // El hijo envía un mensaje al padre
    bsend(PADREPIPE, 1);
}

int main() {
    pid_t pid_hijo;

    // Creo los pipes
    pipe(pipe_fd[PADREPIPE]);
    pipe(pipe_fd[HIJOPIPE]);

    // Configuro los manejadores de señales
    signal(SIGUSR1, handle_sigusr1);
    signal(SIGUSR2, handle_sigusr2);

    pid_hijo = fork();
    if (pid_hijo == 0) {
        hijo();
    }

    // El padre envía la señal inicial al hijo
    int contadorPadre = 1;
    while(1){
        bsend(HIJOPIPE, 0); // Envío la señal al hijo
        breceive(HIJOPIPE); // Espero la señal del hijo
        printf("Soy el padre, contadorPadre: %d\n", contadorPadre);
        contadorPadre+=2;
    }
   

    wait(NULL); // Espera al hijo
    return 0;
}