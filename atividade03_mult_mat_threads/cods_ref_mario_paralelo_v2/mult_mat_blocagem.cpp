#include <iostream>   // std::cout, std::endl
#include <vector>     // std::vector
#include <chrono>     // std::chrono
#include <thread>     // std::thread
#include <algorithm>  // std::min (para limitar o fim do bloco)

//------------------------------------------------------------------------------
// Função auxiliar que processa um intervalo de blocos de colunas [blocoInicio..blocoFim).
// Cada bloco define um intervalo de colunas [jInicio..jFim) de tamanho "BLOCK_SIZE".
//------------------------------------------------------------------------------
void multiplicarBlocagemParalelo(
    const std::vector<std::vector<int>>& matriz, // NxN
    const std::vector<int>& vetor,               // N
    std::vector<int>& resultado,                 // N
    int N,                                       // Dimensão da matriz
    int BLOCK_SIZE,                              // Tamanho do bloco
    int blocoInicio,                             // índice do bloco inicial
    int blocoFim                                 // índice do bloco final (exclusivo)
) {
    // Percorremos os blocos na faixa [blocoInicio..blocoFim).
    // Cada 'bloco b' corresponde às colunas [b * BLOCK_SIZE .. (b+1)*BLOCK_SIZE).
    for (int b = blocoInicio; b < blocoFim; ++b) {
        // Calcula o jInicio e jFim reais em termos de colunas
        int jInicio = b * BLOCK_SIZE;
        // std::min para não passar de N caso o último bloco seja parcial
        int jFim = std::min(jInicio + BLOCK_SIZE, N);

        // Agora percorremos cada linha i
        for (int i = 0; i < N; ++i) {
            // E dentro daquele bloco de colunas (k)
            for (int k = jInicio; k < jFim; ++k) {
                // resultado[i] += matriz[i][k] * vetor[k]
                resultado[i] += matriz[i][k] * vetor[k];
            }
        }
    }
}

int main() {
    const int N = 1000;        // Tamanho NxN
    const int BLOCK_SIZE = 16; // Tamanho do bloco de colunas

    // Matriz NxN com 1s
    std::vector<std::vector<int>> matriz(N, std::vector<int>(N, 1));

    // Vetor N com 1s
    std::vector<int> vetor(N, 1);

    // Resultado N com 0s
    std::vector<int> resultado(N, 0);

    // Definimos quantas threads vamos criar
    int numThreads = 4;

    // Quantos blocos de colunas existem no total?
    // Ex.: se N=1000 e BLOCK_SIZE=16 => 1000/16 = 62.5 => 63 blocos
    int numBlocos = (N + BLOCK_SIZE - 1) / BLOCK_SIZE; // arredondar pra cima

    // Marca início da medição
    auto inicio = std::chrono::high_resolution_clock::now();

    // Vetor para armazenar as threads
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Dividir 'numBlocos' entre 'numThreads'
    int blocosPorThread = numBlocos / numThreads;
    int resto = numBlocos % numThreads;

    int blocoAtual = 0;

    // Criar as threads
    for (int t = 0; t < numThreads; t++) {
        // Se tiver resto, dá 1 bloco extra
        int qtd = blocosPorThread + (t < resto ? 1 : 0);

        int blocoInicio = blocoAtual;
        int blocoFim = blocoInicio + qtd;
        blocoAtual = blocoFim;

        // Cria a thread
        threads.emplace_back(
            multiplicarBlocagemParalelo,
            std::cref(matriz),
            std::cref(vetor),
            std::ref(resultado),
            N,
            BLOCK_SIZE,
            blocoInicio,
            blocoFim
        );
    }

    // Espera todas as threads
    for (auto& th : threads) {
        th.join();
    }

    // Marca o fim
    auto fim = std::chrono::high_resolution_clock::now();
    // Calcula duração
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    std::cout << "Tempo de execucao (blocagem, paralelo): "
              << duracao.count() << " microsegundos" << std::endl;

    return 0;
}

