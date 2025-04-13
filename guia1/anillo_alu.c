#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
// #include "constants.h"
#include <limits.h>

int generate_random_number(){
	return (rand() % 50);
}

int wrapfwd(int i, int n) {
	return (i + 1) % n;
}

void close_ring_pipes_for_process(int id, int pipe_table[][2], int table_size) {
	for (int i = 0; i < table_size; i++) {
		if (i == id) {
			// Close only write end fd
			close(pipe_table[i][1]);
		} else if (i == wrapfwd(id, table_size)) {
			// Close only read end fd
			close(pipe_table[i][0]);
		} else {
			close(pipe_table[i][0]);
			close(pipe_table[i][1]);
		}
	}

}


int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);

	// Create all n pipes of the process ring
	int pipe_fds[n][2];
	for (int i = 0; i < n; i++) {
		if (pipe(pipe_fds[i]) == -1) {
			printf("Pipe %d not created correctly\n", i);
		}
	}

	// Create both father <-> start process pipes
	int pipe_to_start[2];
	int pipe_from_start[2];
	if (pipe(pipe_to_start) == -1) {
		printf("Pipe to start not created correctly\n");
	}
	if (pipe(pipe_from_start) == -1) {
		printf("Pipe from start not created correctly\n");
	}

	// Create all the n ring child processes with fork()
	pid_t child_pid;
	int prev_pipe[2];
	int next_pipe[2];
	int is_start_process = 0;
	for (int i = 0; i < n; i++) {
		child_pid = fork();
		if (child_pid < 0) {
			printf("Fork of child %d failed\n", i);
		} else if (child_pid == 0) {
			// Execute only in child process number i

			// Close all unused ring pipes, copy relevant pipes to variables
			prev_pipe[0] = pipe_fds[i][0];
			prev_pipe[1] = pipe_fds[i][1];
			next_pipe[0] = pipe_fds[(wrapfwd(i,n))][0];
			next_pipe[1] = pipe_fds[(wrapfwd(i,n))][1];
			close_ring_pipes_for_process(i, pipe_fds, n);

			// Close unused father <-> start pipes, if necessary
			if (i == start) {
				// Execute this code only for the start ring process
				is_start_process = 1;
				close(pipe_to_start[1]);
				close(pipe_from_start[0]);
			} else {
				// Execute this code only for non-start ring processes
				close(pipe_to_start[0]);
				close(pipe_to_start[1]);
				close(pipe_from_start[0]);
				close(pipe_from_start[1]);
			}

			// Now all unused pipes should be closed regardless of child process number
			// Now break out of for loop and execute child process code within conditional below
			break;
		}
	}

	if (child_pid != 0) {
		// Only execute this code for father process
		// Close unused pipes for father process
		close(pipe_to_start[0]);
		close(pipe_from_start[1]);
		for (int i = 0; i < n; i++) {
			close(pipe_fds[i][0]);
			close(pipe_fds[i][1]);
		}
	}

	if (child_pid != 0) {
		// Execute main functionality for father process
		// All unused pipe fds are closed

		// I assume that the initial value of the message is the variable named 'buffer'
		int initial_message = buffer;
		write(pipe_to_start[1], &initial_message, sizeof(initial_message));
		close(pipe_to_start[1]);
		int final_value;
		read(pipe_from_start[0], &final_value, sizeof(final_value));
		close(pipe_from_start[0]);
		// Now final_value must be outputted to stdout
		printf("Communication finished with final value: %d", final_value);
		for (int i = 0; i < n; i++) {
			wait(NULL);
		}
		exit(EXIT_SUCCESS);
	}

	if (child_pid == 0 && is_start_process) {
		// Execute main functionality for start child processes
		// All unused pipe fds are closed

		// Read initial value provided by father process
		int initial_value;
		read(pipe_to_start[0], &initial_value, sizeof(initial_value));
		close(pipe_to_start[0]);

		// Generate random secret number that is larger than initial value
		int secret_number = initial_value + 1 + generate_random_number();

		// Read from and write to ring in a loop until a number larger or equal to secret_number is read
		int message = initial_value;
		write(next_pipe[1], &message, sizeof(message));
		while (message < secret_number) {
			if (read(prev_pipe[0], &message, sizeof(message)) == 0) {
				break;
			}
			message += 1;
			write(next_pipe[1], &message, sizeof(message));
		}

		// Initial process has received message number that is bigger or equal to secret number
		// Now it must tell following ring process to close write fd, close its fd, and communicate with father process
		// Close ring pipes, send EOF
		close(prev_pipe[0]);
		close(next_pipe[1]);
		// Communicate with father
		write(pipe_from_start[1], &message, sizeof(message));
		close(pipe_from_start[1]);
		exit(EXIT_SUCCESS);
	}

	if (child_pid == 0 && !is_start_process) {
		// Execute main functionality for non-start child process
		// All unused pipe fds are closed

		int message = 0;
		while (1) {
			if (read(prev_pipe[0], &message, sizeof(message)) == 0) {
				close(prev_pipe[0]);
				close(next_pipe[1]);
				break;
			}
			message += 1;
			write(next_pipe[1], &message, sizeof(message));
		}
	exit(EXIT_SUCCESS);
	}

    /* COMPLETAR */
    
}
