/*
Universidade Presbiteriana Mackenzie
Computação Paralela - Turma 05P11

Alan Meniuk Gleizer
RA 10416804
14/03/2025
*/

/*
Lab 2 - Avaliação de desempenho

### Somatório de números

Faça um programa que some todos os números de um vetor de tamanho 2<sup>30</sup>. Garanta que os números neste vetor sejam sempre os mesmos para garantir o mesmo resultado final!

Construa 2 versões em paralelo utilizando #pragma omp for que soma parcialmente os valores e:
    que utiliza uma variável compartilhada e #pragma omp critial para controle de seção crítica
    que utiliza #pragma omp ... reduction(...) para a somatória final

Faça uma comparação de tempo de execução com 1, 2, 3, 4, 5 e 6 threads, para cada versão. Construa uma tabela contendo o tempo médio de 10 execuções para cada versão e sua respectiva quantidade de threads.
Faça um gráfico que mostre a escalabilidade forte (mínimo de 2<sup>28</sup>, com garantia de todos os dados em memória) e escalabilidade fraca (use 2<sup>30</sup> como quantidade máxima de dados) do Speedup para cada quantidade de threads utilizada.

Entregue os códigos fontes e as tabelas no formato Excel, onde cada aba da planilha é relativa a uma versão diferente de código com sua respectiva tabela de execução e seu respectivo gráfico de speedup.
Submeta prints da execução do seu código.
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N (1 << 30) // 2^30

void carregar_vetor(unsigned int *vetor, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }
    fread(vetor, sizeof(unsigned int), N, file);
    fclose(file);
}

void soma_reduction(unsigned int *vetor, int num_threads)
{
    unsigned long long soma = 0;
    double inicio = omp_get_wtime();

#pragma omp parallel for num_threads(num_threads) reduction(+ : soma)
    for (size_t i = 0; i < N; i++)
    {
        soma += vetor[i];
    }

    double fim = omp_get_wtime();
    printf("Threads: %d | Soma: %llu | Tempo: %.6f segundos\n", num_threads, soma, fim - inicio);
}

int main()
{
    unsigned int *vetor = malloc(N * sizeof(unsigned int));
    if (!vetor)
    {
        perror("Erro ao alocar memória");
        return 1;
    }

    carregar_vetor(vetor, "vetor.bin");

    // Use o vetor normalmente...
    free(vetor);
    return 0;
}