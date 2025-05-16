/*
Universidade Presbiteriana Mackenzie
Computacao Paralela - Turma 05P11

Alan Meniuk Gleizer
RA 10416804
14/03/2025
*/

/*
Lab 3 - - Nova multiplicacao de matrizes
Faca um programa de multiplicacao de matrizes. Fique atento a um bom uso de cache (L1/L2).
>> REIMPLEMENTACAO EM OPENACC <<
*/

/*
PARA COMPILAR
nvc -acc -Minfo=accel -fast lab03_openACC.c -o lab03_openACC -lm

PARA EXECUTAR
./lab03_openACC
*/


#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>
#include <time.h>

#define MAX_TAM 5000  // tamanho maximo
#define NUM_MAX 256 // num max para preenchimento da matriz

// preenche matrix com numero aleatorio entre 0 e numMax
void preencherMatriz(int matriz[MAX_TAM][MAX_TAM], int linhas, int colunas, int numMax) {
    for (int i = 0; i < linhas; i++)
        for (int j = 0; j < colunas; j++)
            matriz[i][j] = rand() % numMax;
}

// aplica convolucao! baseado na multiplicarMatrizPorBlocagem do lab3 original
// como os kernels tem todos os elementos == 1, sequer foi necessário criar uma matriz kernel
void aplicarConvolucao(int entrada[MAX_TAM][MAX_TAM], int saida[MAX_TAM][MAX_TAM], int tam_matriz, int tam_kernel) {
    int i, j, ki, kj;
    int limite = tam_matriz - tam_kernel + 1;

    #pragma acc parallel loop collapse(2) private(ki, kj)
    for (i = 0; i < limite; i++) {
        for (j = 0; j < limite; j++) {
            int soma = 0;
            for (ki = 0; ki < tam_kernel; ki++)
                for (kj = 0; kj < tam_kernel; kj++)
                    soma += entrada[i + ki][j + kj];
            saida[i][j] = soma;
        }
    }
}

int main() {
    int tam_matriz, tam_kernel;

    printf("Defina o tam. da matriz (1048 ou 4096): ");
    scanf("%d", &tam_matriz);

    printf("\nDefina o tam. do kernel (5, 11 ou 33): ");
    scanf("%d", &tam_kernel);

    if (tam_matriz > MAX_TAM || tam_kernel > tam_matriz) {
        printf("ERRO: tamanho invalido!\n");
        return 1;
    }

    // static necessario pois estava ocorrendo stack overflow
    static int entrada[MAX_TAM][MAX_TAM];
    static int saida[MAX_TAM][MAX_TAM];

    srand(time(NULL));
    preencherMatriz(entrada, tam_matriz, tam_matriz, NUM_MAX);

    clock_t inicio = clock();

    #pragma acc data copyin(entrada) copyout(saida)
    {
        aplicarConvolucao(entrada, saida, tam_matriz, tam_kernel);
    }

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) * 1e6 / CLOCKS_PER_SEC;

    printf("\nMatriz tam. %d, kernel tam. %d:\n", tam_matriz, tam_kernel);
    printf("Convolucao concluida com sucesso.\n");
    printf("Tamanho da matriz de saida: %d x %d\n", tam_matriz - tam_kernel + 1, tam_matriz - tam_kernel + 1);
    //printf("Exemplo: saida[0][0] = %d\n", saida[0][0]);
    printf("Tempo de execucao: %.0f microsegundos\n\n", tempo);

    return 0;
}