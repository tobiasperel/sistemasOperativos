#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


void jennifer() {
    printf("Soy Jennifer\n");
    sleep(1);
    printf("mori jennifer\n");
    exit(0);
}

void julieta() {
    printf("Soy Julieta\n");
    sleep(1);
    pid_t pid_jennifer = fork();
    if (pid_jennifer == 0) jennifer();
    printf("mori julieta\n");
    exit(0);
}

int main() {
    pid_t pid_Julieta, pid_Jorge;
    pid_Julieta = fork();
    if (pid_Julieta == 0) julieta();
    printf("Soy Juan\n");
    sleep(1);
    wait(NULL);  // Espera a que Julieta termine
    pid_Jorge = fork();
    if (pid_Jorge == 0) {
        printf("Soy Jorge\n");
        sleep(1);
        printf("mori jorge\n");
        exit(0);
    }
    else{
        printf("mori juan\n");
        exit(0);
    }
    
    return 0;
}
