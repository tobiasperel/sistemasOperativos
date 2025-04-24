<<<<<<< HEAD
#include <stdio.h>
=======
>>>>>>> ea8f1e819724441e8b21a7da66ce68f01c0f50ce
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
<<<<<<< HEAD

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
=======
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

>>>>>>> ea8f1e819724441e8b21a7da66ce68f01c0f50ce
    return 0;
}