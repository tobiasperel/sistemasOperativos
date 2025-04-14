#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>
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
			close(pipe_table[i][1]);
		} else if (i == wrapfwd(id, table_size)) {
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

	int pipe_fds[n][2];
	for (int i = 0; i < n; i++) {
		pipe(pipe_fds[i]);
		
	}

	int pipe_to_start[2];
	int pipe_from_start[2];
	pipe(pipe_to_start);
	pipe(pipe_from_start);

	pid_t child_pid;
	int prev_pipe[2];
	int next_pipe[2];
	bool is_start_process = false;
	for (int i = 0; i < n; i++) {
		child_pid = fork();
		if (child_pid == 0) {

			prev_pipe[0] = pipe_fds[i][0];
			prev_pipe[1] = pipe_fds[i][1];
			next_pipe[0] = pipe_fds[(wrapfwd(i,n))][0];
			next_pipe[1] = pipe_fds[(wrapfwd(i,n))][1];
			close_ring_pipes_for_process(i, pipe_fds, n);

			if (i == start) {
				is_start_process = 1;
				close(pipe_to_start[1]);
				close(pipe_from_start[0]);
			} else {
				close(pipe_to_start[0]);
				close(pipe_to_start[1]);
				close(pipe_from_start[0]);
				close(pipe_from_start[1]);
			}
			break;
		}
	}

	if (child_pid != 0) {
		close(pipe_to_start[0]);
		close(pipe_from_start[1]);
		for (int i = 0; i < n; i++) {
			close(pipe_fds[i][0]);
			close(pipe_fds[i][1]);
		}

		int initial_message = buffer;
		write(pipe_to_start[1], &initial_message, sizeof(initial_message));
		close(pipe_to_start[1]);
		int final_value;
		read(pipe_from_start[0], &final_value, sizeof(final_value));
		close(pipe_from_start[0]);
		printf("Communication finished with final value: %d \n", final_value);
		for (int i = 0; i < n; i++) {
			wait(NULL);
		}
		exit(EXIT_SUCCESS);
	}

	if (child_pid == 0 && is_start_process) {

		int initial_value;
		read(pipe_to_start[0], &initial_value, sizeof(initial_value));
		close(pipe_to_start[0]);

		int secret_number = initial_value + 1 + generate_random_number();

		int message = initial_value;
		write(next_pipe[1], &message, sizeof(message));
		while (message < secret_number) {
			if (read(prev_pipe[0], &message, sizeof(message)) == 0) {
				break;
			}
			message += 1;
			write(next_pipe[1], &message, sizeof(message));
		}

		close(prev_pipe[0]);
		close(next_pipe[1]);

		write(pipe_from_start[1], &message, sizeof(message));
		close(pipe_from_start[1]);
		exit(EXIT_SUCCESS);
	}

	if (child_pid == 0 && !is_start_process) {

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
}
