#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

//------------------------------------------------------------------------------
// Função auxiliar que processa um intervalo de colunas [colunaInicio..colunaFim)
// para multiplicar as matrizes A e B, escrevendo em C.
//------------------------------------------------------------------------------
void multiplicarMatrizesPorColuna(
    const std::vector<std::vector<int>>& A,       // NxN
    const std::vector<std::vector<int>>& B,       // NxN
    std::vector<std::vector<int>>& C,             // NxN (resultado)
    int colunaInicio,
    int colunaFim
) {
    int N = (int)A.size(); // suposição: A.size()==B.size()==C.size()

    // Percorremos as colunas somente na faixa [colunaInicio..colunaFim)
    for (int j = colunaInicio; j < colunaFim; j++) {
        // Para cada linha i
        for (int i = 0; i < N; i++) {
            int soma = 0;
            // Soma sobre k
            for (int k = 0; k < N; k++) {
                soma += A[i][k] * B[k][j];
            }
            C[i][j] = soma;
        }
    }
}

int main() {
    const int N = 1000;

    // Cria matrizes NxN
    std::vector<std::vector<int>> A(N, std::vector<int>(N, 1));
    std::vector<std::vector<int>> B(N, std::vector<int>(N, 1));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    int numThreads = 4;

    auto inicio = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    int colunasPorThread = N / numThreads;
    int resto = N % numThreads;

    int colunaAtual = 0;
    for (int t = 0; t < numThreads; t++) {
        int bloco = colunasPorThread + (t < resto ? 1 : 0);
        int colunaInicio = colunaAtual;
        int colunaFim = colunaInicio + bloco;
        colunaAtual = colunaFim;

        threads.emplace_back(
            multiplicarMatrizesPorColuna,
            std::cref(A),
            std::cref(B),
            std::ref(C),
            colunaInicio,
            colunaFim
        );
    }

    for (auto& th : threads) {
        th.join();
    }

    auto fim = std::chrono::high_resolution_clock::now();
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    std::cout << "Tempo de execucao (mat-mat por coluna, paralelo): "
              << duracao.count() << " microsegundos" << std::endl;

    return 0;
}
