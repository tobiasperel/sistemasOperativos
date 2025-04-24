#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

enum { READ, WRITE };

// Simula un cálculo costoso
int calcular(int numero) {
    sleep(1);
    return numero * 2;
}

// Simula obtener un número secreto para cada PID
int dameNumero(int pid) {
    sleep(1);
    return pid * 3;
}

// Solo lo llama el padre
void informarResultado(int numero, int resultado) {
    printf("El hijo que recibió el número %d devolvió el resultado %d\n", numero, resultado);
}

// Variable global compartida por cada hijo
volatile sig_atomic_t resultadoListo = 0;

// Manejador de señal SIGUSR1
void manejador(int sig) {
    resultadoListo = 1;
}

void ejecutarHijo(int i, int N, int pipes[][2]) {
    signal(SIGUSR1, manejador);

    int numero;
    read(pipes[i][READ], &numero, sizeof(numero)); // recibir número del padre

    int pipeResultado[2];
    pipe(pipeResultado);

    pid_t pid_nieto = fork();
    if (pid_nieto == 0) {
        // Nieto: calcular y enviar resultado
        int resultado = calcular(numero);
        write(pipeResultado[WRITE], &resultado, sizeof(resultado));
        kill(getppid(), SIGUSR1); // avisar al padre (hijo original)
        exit(0);
    }

    close(pipeResultado[WRITE]); // el hijo no escribe el resultado, solo lee

    int resultado = 0;

    // Espera pasiva hasta recibir la señal
    while (!resultadoListo) {
        pause();
    }

    // Lee el resultado del nieto
    read(pipeResultado[READ], &resultado, sizeof(resultado));

    // Esperar polling del padre
    char dummy;
    read(pipes[i][READ], &dummy, sizeof(dummy));

    // Responder afirmativamente y enviar datos
    char termino = 1;
    write(pipes[i + N][WRITE], &termino, sizeof(termino));
    write(pipes[i + N][WRITE], &numero, sizeof(numero));
    write(pipes[i + N][WRITE], &resultado, sizeof(resultado));

    wait(NULL); // esperar al nieto
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Debe ejecutar con la cantidad de hijos como parámetro\n");
        return 1;
    }

    int N = atoi(argv[1]);
    int pipes[N * 2][2];

    // Crear todos los pipes
    for (int i = 0; i < N * 2; i++) {
        pipe(pipes[i]);
    }

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            ejecutarHijo(i, N, pipes);
            return 0;
        } else {
            int numero = dameNumero(pid);
            write(pipes[i][WRITE], &numero, sizeof(numero));
        }
    }

    int cantidadTerminados = 0;
    char *hijoTermino = calloc(N, sizeof(char));

    while (cantidadTerminados < N) {
        for (int i = 0; i < N; i++) {
            if (hijoTermino[i]) continue;

            char dummy = 0;
            write(pipes[i][WRITE], &dummy, sizeof(dummy)); // preguntar si terminó

            char termino = 0;
            read(pipes[i + N][READ], &termino, sizeof(termino));

            if (termino) {
                int numero, resultado;
                read(pipes[i + N][READ], &numero, sizeof(numero));
                read(pipes[i + N][READ], &resultado, sizeof(resultado));
                informarResultado(numero, resultado);
                hijoTermino[i] = 1;
                cantidadTerminados++;
            }
        }
    }

    free(hijoTermino);

    for (int i = 0; i < N; i++) wait(NULL); // esperar todos los hijos

    return 0;
}
