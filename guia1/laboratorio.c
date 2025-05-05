#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

enum { READ, WRITE };
int m; //numero de laboratorios
int n; //numero de estudiantes

int dameLabo(int process_id){
    // Simula obtener un número secreto para cada PID
    // sleep(1);
    return process_id % m;

}

void cerrar_pipes(int pipes[][2], int j) {
    for (int i = 0; i < n; i++) {
        if(i != j) {
            close(pipes[i][READ]);
            close(pipes[i][WRITE]);
        }
    }
}

void hijo(int i, int pipes[][2]) {
    cerrar_pipes(pipes, i); // cerrar los pipes de los otros hijos
    int numeroLabo = dameLabo(getpid());
    write(pipes[i][WRITE], &numeroLabo, sizeof(numeroLabo)); // enviar número al padre
    exit(0);
}

int main(int argc, char *argv[]) {

    n = atoi(argv[1]); //numero de estudiantes
    m = atoi(argv[2]); //numero de laboratorios
    int pipes[n][2];

    for(int i = 0; i < n; i++) {
        pipe(pipes[i]);
    }
    for(int i = 0; i < n; i++) {
        if (fork() == 0) {
            hijo(i, pipes); // devuelve 0 si es el hijo
        } 
    }
    

    for(int i = 0; i < n; i++) {
        int numeroLabo;
        read(pipes[i][READ], &numeroLabo, sizeof(numeroLabo)); // recibir número del hijo
        printf("El hijo %d devolvió el laboratorio %d\n", i, numeroLabo);
    }
    exit(0);

}