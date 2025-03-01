#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm> // std::min

//------------------------------------------------------------------------------
// Função auxiliar que multiplica blocos da matriz A e B, escrevendo em C,
// mas apenas numa faixa de iBlocks (blocos de linhas).
//------------------------------------------------------------------------------
void multiplicarMatrizesBlocagem(
    const std::vector<std::vector<int>>& A,  // NxN
    const std::vector<std::vector<int>>& B,  // NxN
    std::vector<std::vector<int>>& C,        // NxN
    int N,             // dimensão
    int BLOCK_SIZE,    // tamanho do bloco
    int iBlockInicio,  // primeiro bloco de linha a processar
    int iBlockFim      // último bloco de linha (exclusive)
) {
    // Quantidade total de blocos no eixo i (linhas)
    int numBlocksI = (N + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // Quantidade total de blocos no eixo j (colunas)
    int numBlocksJ = (N + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // Quantidade total de blocos no eixo k
    int numBlocksK = (N + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Percorremos os blocos de i apenas na faixa dada [iBlockInicio..iBlockFim)
    for (int bi = iBlockInicio; bi < iBlockFim; bi++) {
        // Calcula limites em i (linhas reais)
        int iInicio = bi * BLOCK_SIZE;
        int iFim = std::min(iInicio + BLOCK_SIZE, N);

        // Percorre todos os blocos de j (colunas)
        for (int bj = 0; bj < numBlocksJ; bj++) {
            int jInicio = bj * BLOCK_SIZE;
            int jFim = std::min(jInicio + BLOCK_SIZE, N);

            // Percorre blocos de k
            for (int bk = 0; bk < numBlocksK; bk++) {
                int kInicio = bk * BLOCK_SIZE;
                int kFim = std::min(kInicio + BLOCK_SIZE, N);

                // Agora percorrer efetivamente as posições (i, j) do sub-bloco
                for (int i = iInicio; i < iFim; i++) {
                    for (int j = jInicio; j < jFim; j++) {
                        // soma parcial
                        int soma = 0;
                        for (int k = kInicio; k < kFim; k++) {
                            soma += A[i][k] * B[k][j];
                        }
                        C[i][j] += soma;
                    }
                }
            }
        }
    }
}

int main() {
    const int N = 1000;
    const int BLOCK_SIZE = 16;

    // Matrizes NxN
    std::vector<std::vector<int>> A(N, std::vector<int>(N, 1));
    std::vector<std::vector<int>> B(N, std::vector<int>(N, 1));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    // Quantidade de threads
    int numThreads = 4;

    // Quantos blocos no eixo i existem no total
    int numBlocksI = (N + BLOCK_SIZE - 1) / BLOCK_SIZE;

    auto inicio = std::chrono::high_resolution_clock::now();

    // Criar threads
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Dividir os blocos de i entre as threads
    int blocosPorThread = numBlocksI / numThreads;
    int resto = numBlocksI % numThreads;

    int blockAtual = 0;
    for (int t = 0; t < numThreads; t++) {
        int qtd = blocosPorThread + ((t < resto) ? 1 : 0);
        int iBlockInicio = blockAtual;
        int iBlockFim = iBlockInicio + qtd;
        blockAtual = iBlockFim;

        threads.emplace_back(
            multiplicarMatrizesBlocagem,
            std::cref(A),
            std::cref(B),
            std::ref(C),
            N,
            BLOCK_SIZE,
            iBlockInicio,
            iBlockFim
        );
    }

    for (auto& th : threads) {
        th.join();
    }

    auto fim = std::chrono::high_resolution_clock::now();
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    std::cout << "Tempo de execucao (mat-mat blocagem, paralelo): "
              << duracao.count() << " microsegundos" << std::endl;

    return 0;
}
