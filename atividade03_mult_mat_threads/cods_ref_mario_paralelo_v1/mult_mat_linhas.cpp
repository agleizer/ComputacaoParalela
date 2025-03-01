#include <iostream>   // Para std::cout, std::endl
#include <vector>     // Para std::vector
#include <chrono>     // Para medir tempo (std::chrono)
#include <thread>     // Para std::thread

//------------------------------------------------------------------------------
// Função auxiliar que cada thread executa para multiplicar uma faixa de linhas
// da matriz A por B, armazenando o resultado em C.
// Faixa de linhas: [linhaInicio..linhaFim)
//------------------------------------------------------------------------------
void multiplicarMatrizesPorLinha(
    const std::vector<std::vector<int>>& A,      // Matriz A (NxN)
    const std::vector<std::vector<int>>& B,      // Matriz B (NxN)
    std::vector<std::vector<int>>& C,            // Matriz C (NxN), resultado
    int linhaInicio,                             // Linha inicial (inclusive)
    int linhaFim                                 // Linha final (exclusive)
) {
    // Tamanho N da matriz (A, B, C)
    int N = (int)A.size();  // supomos que A.size() == B.size() == C.size()

    // Percorre as linhas na faixa dada
    for (int i = linhaInicio; i < linhaFim; i++) {
        // Percorre todas as colunas de C
        for (int j = 0; j < N; j++) {
            int soma = 0;
            // Soma parcial em k
            for (int k = 0; k < N; k++) {
                soma += A[i][k] * B[k][j];
            }
            // Armazena em C[i][j]
            C[i][j] = soma;
        }
    }
}

int main() {
    // Definimos N, o tamanho das matrizes NxN
    const int N = 1000;

    // Criar matrizes A, B (NxN) preenchidas com 1
    std::vector<std::vector<int>> A(N, std::vector<int>(N, 1));
    std::vector<std::vector<int>> B(N, std::vector<int>(N, 1));

    // Cria a matriz C (resultado), inicializada com 0
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    // Definimos o número de threads
    int numThreads = 4;

    // Marca o tempo inicial
    auto inicio = std::chrono::high_resolution_clock::now();

    // Vetor para armazenar as threads
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Quantas linhas cada thread irá processar
    int linhasPorThread = N / numThreads;
    int resto = N % numThreads;

    int linhaAtual = 0;

    // Criamos as threads, cada uma processando um intervalo [linhaInicio..linhaFim)
    for (int t = 0; t < numThreads; t++) {
        int bloco = linhasPorThread + (t < resto ? 1 : 0);
        int linhaInicio = linhaAtual;
        int linhaFim = linhaInicio + bloco;
        linhaAtual = linhaFim;

        // Lança a thread
        threads.emplace_back(
            multiplicarMatrizesPorLinha,
            std::cref(A),
            std::cref(B),
            std::ref(C),
            linhaInicio,
            linhaFim
        );
    }

    // Aguarda todas as threads terminarem
    for (auto& th : threads) {
        th.join();
    }

    // Marca o tempo final
    auto fim = std::chrono::high_resolution_clock::now();
    // Calcula a duração em microsegundos
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    // Exibe o tempo de execução
    std::cout << "Tempo de execucao (mat-mat por linha, paralelo): "
              << duracao.count() << " microsegundos" << std::endl;

    return 0;
}
