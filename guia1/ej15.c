#include <stdio.h>    // printf()
#include <stdlib.h>   // exit()
#include <unistd.h>   // fork() pipe() execlp() dup2() close()
#include <sys/wait.h> // wait()
int pipe_fd[2];

// Constants 0 and 1 for READ and WRITE
enum { READ, WRITE };

// Debe ejecutar "ls -al"
void ejecutar_hijo_1(int pipe_fd[]) {
    // Redirigir la salida estándar al pipe
    dup2(pipe_fd[WRITE], STDOUT_FILENO);
    // Cerrar el pipe de escritura
    close(pipe_fd[READ]); //cerrar el pipe de lectura ya que solo escribe
    close(pipe_fd[WRITE]); //como ya copie el puntero ahora stdout apunta a pipe_fd[WRITE]
    // Ejecutar el comando "ls -al"
    execlp("ls", "ls", "-al", NULL);
    exit(0);
}

// Debe ejecutar "wc -l"
void ejecutar_hijo_2(int pipe_fd[]) {
    // Redirigir la entrada estándar al pipe
    dup2(pipe_fd[READ], STDIN_FILENO);
    // Cerrar el pipe de lectura
    close(pipe_fd[WRITE]); // cerrar el pipe de escritura ya que solo lee
    close(pipe_fd[READ]);  // esta linea es fundamenta cerrar ya que sino wc /l se queda congelado ya que pueden seguir escribiendo. como ya copie el puntero ahora stdin apunta a pipe_fd[READ]
    // Ejecutar el comando "wc -l"
    execlp("wc", "wc", "-l", NULL);
    exit(0);
}

int main(int argc, char const* argv[]) {
    // Crear el pipe
    pipe(pipe_fd);
    // Crear el primer hijo
    pid_t pid_hijo_1 = fork();
    if (pid_hijo_1 == 0) ejecutar_hijo_1(pipe_fd);
    // Crear el segundo hijo
    pid_t pid_hijo_2 = fork();
    if (pid_hijo_2 == 0) ejecutar_hijo_2(pipe_fd);
    // Cerrar los pipes en el padre
    close(pipe_fd[READ]);
    close(pipe_fd[WRITE]);
    // Esperar a que terminen los hijos
    wait(NULL);
    wait(NULL);
    return 0;
}
