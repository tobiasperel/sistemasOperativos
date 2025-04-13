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

	while (1) {
        fflush(stdout);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break; // quizas hay que sacarlo
        
        write(sockfd, &buffer, sizeof(buffer));
        if (strncmp(buffer, "exit", 4) == 0) break;

        memset(buffer, 0, BUFFER_SIZE);
         ssize_t n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
         if (n <= 0) {
             printf("El servidor cerró la conexión.\n");
             break;
         }

        printf("Respuesta: %s", buffer);
    }
    close(server_socket);
	exit(0);
}
