// Autor: Juan Agustin Avila
int pipe_fd[2][2]; // 0 es para leer, 1 es para escribir son dos pipes(uno para cada hijo)

enum {READ, WRITE};
enum {LESTER, ELIZA};

int tirar_dado(){
    return rand() % 6 + 1;
}

void lester(){
    int dado = tirar_dado();
    write(pipe_fd[LESTER][WRITE], &dado, sizeof(dado));
    exit(0);
}

void eliza(){
    int dado = tirar_dado();
    write(pipe_fd[ELIZA][WRITE], &dado, sizeof(dado));
    exit(0);
}

int main(int argc, char const *argv[]){
    //creo el pipe
    pipe(pipe_fd[LESTER]);
    pipe(pipe_fd[ELIZA]);

    if(fork() == 0) lesster(); // devuelve 0 si es el hijo
    if(fork() == 0) eliza(); // devuelve 0 si es el hijo

    int dado_lester, dado_eliza;
    read (pipe_fd[LESTER][READ], &dado_lester, sizeof(dado_lester)); 
    read (pipe_fd[ELIZA][READ], &dado_eliza, sizeof(dado_eliza));

    printf("Lester tiro %d\n", dado_lester);
    printf("Eliza tiro %d\n", dado_eliza);
    printf("Gano %s\n", dado_lester > dado_eliza ? "Lester" : "Eliza");
}
