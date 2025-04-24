#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define BUFFER_SIZE 1024
int calcular(const char *expresion) {
    int num1, num2, resultado;
    char operador;

    // Usamos sscanf para extraer los dos números y el operador de la expresión
    if (sscanf(expresion, "%d%c%d", &num1, &operador, &num2) != 3) {
        printf("Formato incorrecto\n");
        return 0;  // En caso de error, retornamos 0.
    }

    // Realizamos la operación según el operador
    switch (operador) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                resultado = num1 / num2;
            } else {
                printf("Error: División por cero\n");
                return 0;  // Si hay división por cero, retornamos 0.
            }
            break;
        default:
            printf("Operador no reconocido\n");
            return 0;  // Si el operador no es válido, retornamos 0.
    }

    return resultado;
}


void hijo(int client_socket){
    char buffer[BUFFER_SIZE];
    int result;
    while (1){
        ssize_t bytes_recibidos = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        printf("bytes recibidos: %ld\n", bytes_recibidos); // si hace control c
        if (bytes_recibidos <= 0) break;
        if(strlen(buffer) == 0) continue;
        if (strcmp(buffer, "exit") == 0) break;
        result = calcular(buffer);
        send(client_socket, &result, sizeof(result), 0);
    }
    close(client_socket);
    exit(0);
}

int main() {
     
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
    while(1) {

        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &clen);
        if (client_socket < 0) continue;

        pid_t pid = fork();
        if (pid == 0) {
            close(server_socket); // el hijo no necesita escuchar más conexiones
            hijo(client_socket);

        } else {
            close(client_socket); // el padre no atiende al cliente directamente
        }

    }

    exit(0);
}

