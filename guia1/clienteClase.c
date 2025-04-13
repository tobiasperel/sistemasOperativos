#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int server_socket;
    struct sockaddr_un server_addr;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error");
        exit(1);
    }

    int num = 42;
    char operacion[6];
    strcpy(operacion, "4 + 3");
    write(server_socket, &operacion, sizeof(operacion));
    read(server_socket, &operacion, sizeof(operacion));
    // printf("Cliente: recibí %d del servidor!\n", operacion);

    close(server_socket);
    exit(0);
}
