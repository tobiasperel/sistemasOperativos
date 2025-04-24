#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/wait.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 128

void manejarOperacion(const char *input, char *output) {
    double a, b;
    char op;
    if (sscanf(input, "%lf %c %lf", &a, &op, &b) != 3) {
        snprintf(output, BUFFER_SIZE, "Formato inválido. Usa: <número> <op> <número>\n");
        return;
    }

    switch (op) {
        case '+':
            snprintf(output, BUFFER_SIZE, "%.2lf\n", a + b);
            break;
        case '-':
            snprintf(output, BUFFER_SIZE, "%.2lf\n", a - b);
            break;
        case '*':
            snprintf(output, BUFFER_SIZE, "%.2lf\n", a * b);
            break;
        case '/':
            if (b == 0)
                snprintf(output, BUFFER_SIZE, "Error: división por cero\n");
            else
                snprintf(output, BUFFER_SIZE, "%.2lf\n", a / b);
            break;
        default:
            snprintf(output, BUFFER_SIZE, "Operador inválido. Usa + - * /\n");
            break;
    }
}

void atenderCliente(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_recibidos = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_recibidos <= 0) break;

        buffer[strcspn(buffer, "\n")] = 0;  // quitar salto de línea

        if (strcmp(buffer, "exit") == 0) break;

        char respuesta[BUFFER_SIZE];
        manejarOperacion(buffer, respuesta);
        send(client_socket, respuesta, strlen(respuesta), 0);
    }

    close(client_socket);
    exit(0);  // termina hijo
}

void reapChildren(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_un server_addr, client_addr;
    socklen_t clen = sizeof(client_addr);

    signal(SIGCHLD, reapChildren);  // evitar zombies

    unlink(SOCKET_PATH);  // borrar socket anterior
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 5);

    printf("Servidor esperando conexiones...\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &clen);
        if (client_socket < 0) continue;

        pid_t pid = fork();
        if (pid == 0) {
            close(server_socket); // el hijo no necesita escuchar más conexiones
            atenderCliente(client_socket);
        } else {
            close(client_socket); // el padre no atiende al cliente directamente
        }
    }

    close(server_socket);
    unlink(SOCKET_PATH);
    return 0;
}
