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

Este programa carrega um vetor de tamanho 2^30 de um arquivo binário e realiza a soma de seus elementos
de forma paralela utilizando OpenMP. Ele implementa duas versões:

1. Versão usando `#pragma omp critical` para proteger a soma compartilhada.
2. Versão usando `#pragma omp reduction(+:soma)`, que é mais eficiente.

O programa recebe o número de threads como argumento e compara o tempo de execução para cada abordagem.
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N (1 << 30) // uso de bitwise shift left para desempenho!

void carregar_vetor(unsigned int *vetor, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }

    // Lê os valores do arquivo binário para a memória
    fread(vetor, sizeof(unsigned int), N, file);
    fclose(file);
}

void soma_critical(unsigned int *vetor, int num_threads)
{
    unsigned long long soma = 0;     // Variável compartilhada para armazenar a soma total
    double inicio = omp_get_wtime(); // Marca o tempo inicial

#pragma omp parallel num_threads(num_threads) // Inicia a região paralela
    {
        unsigned long long parcial = 0; // Variável local para soma parcial de cada thread

#pragma omp for // Divide o loop entre as threads
        for (size_t i = 0; i < N; i++)
        {
            parcial += vetor[i]; // Cada thread acumula sua soma local
        }

// Região crítica para evitar condições de corrida ao atualizar a soma global
#pragma omp critical
        soma += parcial;
    }

    double fim = omp_get_wtime(); // Marca o tempo final
    printf("CRITICAL | Threads: %d | Soma: %llu | Tempo: %.6f segundos\n", num_threads, soma, fim - inicio);
}

void soma_reduction(unsigned int *vetor, int num_threads)
{
    unsigned long long soma = 0;     // Variável compartilhada para armazenar a soma total
    double inicio = omp_get_wtime(); // Marca o tempo inicial

// Paraleliza o loop e usa `reduction(+:soma)` para evitar a necessidade de `critical`
#pragma omp parallel for num_threads(num_threads) reduction(+ : soma)
    for (size_t i = 0; i < N; i++)
    {
        soma += vetor[i]; // Cada thread acumula sua parte diretamente na variável reduction
    }

    double fim = omp_get_wtime(); // Marca o tempo final
    printf("REDUCTION | Threads: %d | Soma: %llu | Tempo: %.6f segundos\n", num_threads, soma, fim - inicio);
}

int main(int argc, char *argv[])
{
    // Verifica se o número correto de argumentos foi passado
    if (argc != 2)
    {
        printf("Uso: %s <num_threads>\n", argv[0]);
        return 1;
    }

    // Converte o argumento para inteiro (número de threads)
    int num_threads = atoi(argv[1]);
    if (num_threads < 1)
    {
        printf("Número de threads inválido: %d\n", num_threads);
        return 1;
    }

    // Aloca memória para o vetor
    unsigned int *vetor = (unsigned int *)malloc(N * sizeof(unsigned int));
    if (!vetor)
    {
        perror("Erro ao alocar memória");
        return 1;
    }

    // Carrega o vetor a partir do arquivo binário
    carregar_vetor(vetor, "vetor.bin");

    // Executa e mede o tempo das duas versões
    soma_critical(vetor, num_threads);
    soma_reduction(vetor, num_threads);

    // Libera a memória alocada
    free(vetor);

    return 0;
}
