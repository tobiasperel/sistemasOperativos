#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int result;
pid_t pid_izquierda, pid_derecha;
int cómputo_muy_difícil_1()
{
    // Simula un cómputo difícil
    sleep(1);
    return rand() % 100;
}
int cómputo_muy_difícil_2()
{
    // Simula un cómputo difícil
    sleep(1);
    return rand() % 100;
}
void proceso_izquierda()
{
    result = 0;
    while (1)
    {
        bsend(pid_derecha, result);
        result = cómputo_muy_difícil_1();
    }
}
void proceso_derecha()
{
    while (1)
    {
        result = cómputo_muy_difícil_2();
        int left_result = breceive(pid_izquierda);
        printf("%s %s", left_result, result);
    }
}