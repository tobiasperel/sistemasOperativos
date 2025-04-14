#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUFFER_SIZE 1024

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

    char buffer[BUFFER_SIZE]; // Declare the buffer variable
    int resultado;
    while (1) {
        printf("Conectado al servidor. Ingrese expresiones (ej: 10+5) o 'exit' para salir:\n");

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break; // quizas hay que sacarlo
        
        write(server_socket, &buffer, sizeof(buffer));
        if (strncmp(buffer, "exit", 4) == 0) break;

        read(server_socket, &resultado, sizeof(resultado));
          
        printf("Respuesta: %d\n", resultado);
    }
    close(server_socket);
	exit(0);
}
