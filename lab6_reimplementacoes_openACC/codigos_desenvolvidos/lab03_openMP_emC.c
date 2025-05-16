/*
Universidade Presbiteriana Mackenzie
Computação Paralela - Turma 05P11

Alan Meniuk Gleizer
RA 10416804
14/03/2025
*/

/*
Lab 3 - - Nova multiplicação de matrizes
Faça um programa de multiplicação de matrizes. Fique atento a um bom uso de cache (L1/L2)
Construa uma versão sequencial e uma versão paralela com OpenMP, utilizando alguma das técnicas estududas e/ou apresentadas na literatura.

Faça uma comparação de tempo de execução com 1, 2, 3, 4, 5 e 6 threads, para cada versão. Construa uma tabela contendo o tempo médio de 3 execuções para cada versão e sua respectiva quantidade de threads.
Faça um gráfico que mostre a escalabilidade do Speedup para cada quantidade de threads utilizada.

Entregue os códigos fontes e as tabelas no formato Excel, onde cada aba da planilha é relativa a uma versão diferente de código com sua respectiva tabela de execução.
Submeta também os prints da execução.
*/


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

#define N 1000
#define BLOCK_SIZE 16

void multiplicarMatrizesPorBlocagem(int A[N][N], int B[N][N], int C[N][N]) {
    int i, jBlock, j, k;

    #pragma omp parallel for private(jBlock, j, k)
    for (i = 0; i < N; i++) {
        for (jBlock = 0; jBlock < N; jBlock += BLOCK_SIZE) {
            int jFim = jBlock + BLOCK_SIZE < N ? jBlock + BLOCK_SIZE : N;
            for (j = jBlock; j < jFim; j++) {
                int soma = 0;
                for (k = 0; k < N; k++) {
                    soma += A[i][k] * B[k][j];
                }
                C[i][j] = soma;
            }
        }
    }
}

long long microsegundos() {
    struct timeval tempo;
    gettimeofday(&tempo, NULL);
    return (long long)tempo.tv_sec * 1000000LL + tempo.tv_usec;
}

int main(int argc, char* argv[]) {
    int A[N][N], B[N][N], C[N][N];
    int i, j;

    // Inicializa A e B com 5 e C com 0
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = 5;
            B[i][j] = 5;
            C[i][j] = 0;
        }
    }

    int numThreads = 1; // Padrão: 1 thread
    if (argc > 1) {
        numThreads = atoi(argv[1]); // Converte argumento para inteiro
        if (numThreads < 1) {
            fprintf(stderr, "Número de threads inválido! Usando 1 thread.\n");
            numThreads = 1;
        }
    }
    omp_set_num_threads(numThreads);

    long long inicio = microsegundos();

    multiplicarMatrizesPorBlocagem(A, B, C);

    long long fim = microsegundos();
    long long duracao = fim - inicio;

    printf("\nC[0][0] = %d\n", C[0][0]);
    printf("Tempo de execucao (blocagem, OpenMP) com %d threads: %lld microsegundos\n\n", numThreads, duracao);

    return 0;
}
