#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define RANGO_MIN 2L
#define RANGO_MAX 1000000001L

// Constants 0 and 1 for READ and WRITE
enum { READ, WRITE };

int procesos;

int esPar(long numero) {
  return (numero & 1) == 0;
}

long contarPares(long desde, long hasta) {
  long cantidad = 0;
  for (long i = desde; i < hasta; ++i) {
    if (esPar(i)) {
      cantidad++;
    }
  }
  return cantidad;
}

void ejecutarHijo( int client_socket) {
  // Leer del i-ésimo pipe el rango [desde, hasta) para realizar el cómputo
  // Contar pares y escribir el resultado
  int resultado;
  if (op == '+') {
    resultado = n1 + n2;
  } else if (op == '-') {
    resultado = n1 - n2;
  } else if (op == '*') {
    resultado = n1 * n2;
  } else if (op == '/') {
    if (n2 != 0) {
      resultado = n1 / n2;
    } else {
      printf("Error: División por cero\n");
      exit(EXIT_FAILURE);
    }
  } else {
    printf("Operador inválido\n");
    exit(EXIT_FAILURE);
  }
  send(client_socket, &resultado, sizeof(resultado), 0);

}

int n1;
int n2;
char op;

void decodificar(char* operacion) {
  char* token = strtok(operacion, " ");
  n1 = atoi(token);
  token = strtok(NULL, " ");
  op = *token;
  token = strtok(NULL, " ");
  n2 = atoi(token);
}

int main(int argc, char const* argv[]) {
  // Parsear la cantidad de procesos
  if (argc != 2) {
    printf("Debe ejecutar con la cantidad de procesos N como parámetro.\n");
    printf("Ejemplo: %s N\n", argv[0]);
    return 1;
  }
  procesos = atoi(argv[1]);

   int server_socket;
    int client_socket;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    unsigned int slen = sizeof(server_addr);
    unsigned int clen = sizeof(client_addr);
    int num;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");
    unlink(server_addr.sun_path);

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, slen);
    listen(server_socket, 1);

    printf("Servidor: esperando conexión del cliente...\n");
  // Crear pipes
    int pipes[procesos][2];
    for (int i = 0; i < procesos; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error al crear pipe");
            exit(EXIT_FAILURE);
        }
    }
    char operacion[50];

  // Crear hijos
  while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &clen);
        if (client_socket < 0) continue;
        recv(client_socket, &operacion, sizeof(operacion), 0);
        //mostrar largo de la operacion
        printf("Servidor: recibí %s del cliente!\n", operacion);
        pid_t pid = fork();
        if (pid == 0) {
          decodificar(operacion);
          
            close(server_socket); // el hijo no necesita escuchar más conexiones 
            // write(pipes[i][WRITE], &client_socket, sizeof(client_socket)); // Enviar socket al hijo      
            ejecutarHijo(client_socket); // Aquí se ejecuta la función del hijo
        } else {
            close(client_socket); // el padre no atiende al cliente directamente
        }
    }

  

  

  return 0;
}
