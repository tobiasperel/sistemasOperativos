#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>


void Ke_context_switch(PCB* pcb_0, PCB* pcb_1) {
// Tomando el programa en C que diste como ejemplo, podemos mapear los pasos del cambio de contexto del ejercicio 1:

//     Guardar el estado del proceso saliente:
//     → En este programa no se ve explícitamente porque está a nivel de usuario, pero sería parte de Ke_context_switch en kernel.

//     Actualizar el estado del proceso saliente:
//     → En Ke_context_switch, se actualiza pcb_0->STAT = KE_READY.

//     Seleccionar el nuevo proceso:
//     → Implícito en set_current_process(pcb_1->P_ID);.

//     Restaurar el estado del nuevo proceso:
//     → Se cargan los registros R0–R15 y el PC desde pcb_1.

//     Reiniciar la ejecución:
//     → Se hace con ret();, que salta al nuevo PC.


    // Guardar el estado del proceso saliente (pcb_0)
    pcb_0->PC = PC;
    pcb_0->R0 = R0;
    pcb_0->R1 = R1;
    pcb_0->R2 = R2;
    pcb_0->R3 = R3;
    pcb_0->R4 = R4;
    pcb_0->R5 = R5;
    pcb_0->R6 = R6;
    pcb_0->R7 = R7;
    pcb_0->R8 = R8;
    pcb_0->R9 = R9;
    pcb_0->R10 = R10;
    pcb_0->R11 = R11;
    pcb_0->R12 = R12;
    pcb_0->R13 = R13;
    pcb_0->R14 = R14;
    pcb_0->R15 = R15;
    
    // Guardar el tiempo de CPU usado por pcb_0
    pcb_0->CPU_TIME += ke_current_user_time();

    // Actualizar su estado
    pcb_0->STAT = KE_READY;

    // Preparar el nuevo proceso (pcb_1)
    pcb_1->STAT = KE_RUNNING;

    // Asignar nuevo proceso al scheduler
    set_current_process(pcb_1->P_ID);

    // Restaurar registros del nuevo proceso
    R0 = pcb_1->R0;
    R1 = pcb_1->R1;
    R2 = pcb_1->R2;
    R3 = pcb_1->R3;
    R4 = pcb_1->R4;
    R5 = pcb_1->R5;
    R6 = pcb_1->R6;
    R7 = pcb_1->R7;
    R8 = pcb_1->R8;
    R9 = pcb_1->R9;
    R10 = pcb_1->R10;
    R11 = pcb_1->R11;
    R12 = pcb_1->R12;
    R13 = pcb_1->R13;
    R14 = pcb_1->R14;
    R15 = pcb_1->R15;

    // Restaurar el contador de programa
    PC = pcb_1->PC;

    // Resetear cronómetro
    ke_reset_current_user_time();

    // Retomar ejecución
    ret();
}



int main(int argc, char const *argv[])
{
	pid_t pid_h1;
	pid_t pid_h2;

	pid_h1 = fork();
	if(pid_h1 == 0) hijo1();

	pid_h2 = fork();
	if(pid_h2 == 0) hijo2();

	int numero; 
	scanf("%d", &numero);
	printf("envío la señal número %d al hijo 1\n", numero); 
	kill(pid_h1, numero);
	printf("envío la señal número %d al hijo 2\n", numero); 
	kill(pid_h2, numero);


	exit(EXIT_SUCCESS);
}