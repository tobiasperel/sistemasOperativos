#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>

pid_t pid_padre, pid_hijo;

int pipe_fd[2][2]; 
enum {READ, WRITE};
enum {PADREPIPE, HIOPIPE};

void signal_handler_hijo(int signo) {
    int value;
    read(pipe_fd[PADREPIPE][READ], &value, sizeof(int)); // Lee del pipe
    printf("El hijo recibio %d y envia %d\n", value, value + 1); // Corrige el formato
    value++;
    write(pipe_fd[HIOPIPE][WRITE], &value, sizeof(int)); // Escribe en el pipe
    if(value >= 10){
        printf("El hijo termina\n");
        exit(0); // Termina el hijo
    }
    kill(pid_padre, SIGUSR1); // Envía la señal SIGUSR2 al padre
}
void signal_handler_padre(int signo) {
    int value;
    read(pipe_fd[HIOPIPE][READ], &value, sizeof(int)); // Lee del pipe
    printf("El padre recibio %d y envia %d\n", value, value + 1); // Corrige el formato
    value++;
    write(pipe_fd[PADREPIPE][WRITE], &value, sizeof(int)); // Escribe en el pipe
    kill(pid_hijo, SIGUSR1); // Envía la señal SIGUSR1 al hijo
}

// Implementación de breceive
void breceive(pid_t sender_pid) {
    signal(SIGUSR1, signal_handler_hijo); // Establece el manejador de señales
    printf("El hijo espera la señal del padre\n");
    pause(); // Espera la señal
}

// Implementación de bsend
void bsend(pid_t receiver_pid) {
    kill(receiver_pid, SIGUSR1); // Envía la señal SIGUSR1
    printf("El padre envia la señal al hijo\n");
    pause(); // Espera la señal
}

void hijo() {
    breceive(pid_padre); 
    exit(0);
}

int main() {
    pipe(pipe_fd[PADREPIPE]); // Crea el pipe para el padre
    pipe(pipe_fd[HIOPIPE]); // Crea el pipe para el hijo
    pid_padre = getpid();
    pid_hijo = fork();

    if (pid_hijo == 0)  hijo(); 
    int value = 0;
    signal(SIGUSR1, signal_handler_padre); // Establece el manejador de señales
    write(pipe_fd[PADREPIPE][WRITE], &value, sizeof(int)); // Escribe en el pipe
    bsend(pid_hijo); 
    wait(NULL); 

    return 0;
}