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

#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

// OBS: código realiza multiplicação de matrizes QUADRADAS de mesmo tamanho para simplificar

void multiplicarMatrizesPorBlocagem(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B, std::vector<std::vector<int>>& C) {
    int N = (int)A.size();
    const int BLOCK_SIZE = 16;

    #pragma omp parallel for collapse(2)
    // uso do collapse: fundir múltiplos loops aninhados em um único loop e permitir que o OpenMP distribua as iterações entre as threads
    // o 2 especifica que vamos fundir 2 lops
    for (int i = 0; i < N; i++) {
        for (int jBlock = 0; jBlock < N; jBlock += BLOCK_SIZE) {
            int jFim = std::min(jBlock + BLOCK_SIZE, N);
            for (int j = jBlock; j < jFim; j++) {
                int soma = 0;
                for (int k = 0; k < N; k++) {
                    soma += A[i][k] * B[k][j];
                }
                C[i][j] = soma;
            }
        }
    }
}

int main() {
    const int N = 500;
    std::vector<std::vector<int>> A(N, std::vector<int>(N, 5));
    std::vector<std::vector<int>> B(N, std::vector<int>(N, 5));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    int numThreads = 10;
    omp_set_num_threads(numThreads);

    auto inicio = std::chrono::high_resolution_clock::now();
    
    multiplicarMatrizesPorBlocagem(A, B, C);
    
    auto fim = std::chrono::high_resolution_clock::now();
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    std::cout << "\nC[0][0] = " << C[0][0] << std::endl;
    std::cout << "Tempo de execucao (blocagem, OpenMP): " << duracao.count() << " microsegundos\n" << std::endl;
    
    return 0;
}
