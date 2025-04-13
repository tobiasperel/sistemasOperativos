#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "unix_socket"
#define BUFFER_SIZE 128

int main() {
    int sockfd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    // Crear socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    // Configurar dirección
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Conectar al servidor
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar con el servidor");
        close(sockfd);
        exit(1);
    }

    printf("Conectado al servidor. Escribí operaciones (ej: 3 + 4) o 'exit' para salir.\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break;
        //hago un string con la data qeu diga 4 + 3
        char operacion[6];
        strcpy(operacion, "4 + 3");
        write(sockfd, &operacion, sizeof(operacion));

        if (strncmp(buffer, "exit", 4) == 0) break;

        memset(buffer, 0, BUFFER_SIZE);
        ssize_t n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0) {
            printf("El servidor cerró la conexión.\n");
            break;
        }

        printf("Respuesta: %s", buffer);
    }

    close(sockfd);
    return 0;
}
