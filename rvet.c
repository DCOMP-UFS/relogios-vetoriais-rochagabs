/**
 * Código base (incompleto) para implementação de relógios vetoriais.
 * Meta: implementar a interação entre três processos ilustrada na figura
 * da URL: 
 * 
 * https://people.cs.rutgers.edu/~pxk/417/notes/images/clocks-vector.png
 * 
 * Compilação: mpicc -o rvet rvet.c
 * Execução:   mpiexec -n 3 ./rvet
 */

#include <stdio.h>
#include <string.h>
#include <mpi.h>

typedef struct Clock {
    int p[3];
} Clock;

int max(int a, int b) {
    return a > b ? a : b;
}

void Event(int pid, Clock *clock) {
    clock->p[pid]++; //evento ocorreu
}

void Send(int pid, Clock *clock) {
    MPI_Send(&(clock->p), 3, MPI_INT, pid, 1, MPI_COMM_WORLD);
    //printf("Process %d will receive Clock: (%d, %d, %d)\n", pid, clock->p[0], clock->p[1], clock->p[2]); //adicionada

}

void Receive(int pid, Clock *clock) {
    int received_clock[3];

    MPI_Recv(received_clock, 3, MPI_INT, pid, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = 0; i < 3; i++) {
        clock->p[i] = max(clock->p[i], received_clock[i]);
    }

    //printf("Process %d sent  message. Clock: (%d, %d, %d)\n", pid, clock->p[0], clock->p[1], clock->p[2]);

}

// Representa o processo de rank 0
void process0() {
    Clock clock = {{0, 0, 0}};
    Event(0, &clock);

    Event(0, &clock); // Incrementa o relógio para enviar
    Send(1, &clock);

    Receive(1, &clock);
    Event(0, &clock); // Incrementa o relógio após a recepção

    Event(0, &clock); // Incrementa o relógio para enviar
    Send(2, &clock);

    Receive(2, &clock);
    Event(0, &clock); // Incrementa o relógio após a recepção

    Event(0, &clock); // Incrementa o relógio para enviar
    Send(1, &clock);

    Event(0, &clock); // Incrementa o relógio após o evento
    printf("Process: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
}


// Representa o processo de rank 1
void process1() {
    Clock clock = {{0, 0, 0}};


    Event(1, &clock); // Incrementa o relógio para enviar
    Send(0, &clock);

    Receive(0, &clock);
    Event(1, &clock); // Incrementa o relógio após a recepção

    Receive(0, &clock);
    Event(1, &clock); // Incrementa o relógio após a recepção

    printf("Process: %d, Clock: (%d, %d, %d)\n", 1, clock.p[0], clock.p[1], clock.p[2]);
}

// Representa o processo de rank 2
void process2() {
    Clock clock = {{0, 0, 0}};
    Event(2, &clock);

    Event(2, &clock); // Incrementa o relógio para enviar
    Send(0, &clock);


    Receive(0, &clock);
    Event(2, &clock); // Incrementa o relógio após a recepção

    printf("Process: %d, Clock: (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);
}

int main(void) {
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        process0();
    } else if (my_rank == 1) {
        process1();
    } else if (my_rank == 2) {
        process2();
    }

    /* Finaliza MPI */
    MPI_Finalize();

    return 0;
}  /* main */
