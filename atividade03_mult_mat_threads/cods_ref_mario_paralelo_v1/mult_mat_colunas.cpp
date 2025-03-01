#include <iostream>   // std::cout, std::endl
#include <vector>     // std::vector
#include <chrono>     // std::chrono para medir tempo
#include <thread>     // std::thread
#include <cmath>      // se precisarmos de funções matemáticas extras

//------------------------------------------------------------------------------
// Função auxiliar que processa um intervalo de colunas [colunaInicio..colunaFim).
// Cada thread chamará essa função, varrendo primeiro as colunas (j) para depois as linhas (i).
//------------------------------------------------------------------------------
void multiplicarPorColunaParalelo(
    const std::vector<std::vector<int>>& matriz, // (entrada) NxN
    const std::vector<int>& vetor,               // (entrada) N
    std::vector<int>& resultado,                 // (saída) N
    int colunaInicio,                            // coluna inicial
    int colunaFim                                // coluna final (exclusivo)
) {
    // Vamos supor que matriz.size() == N e matriz[i].size() == N.
    int N = (int)matriz.size();

    // Loop externo pelas colunas, mas somente na faixa [colunaInicio..colunaFim).
    for (int j = colunaInicio; j < colunaFim; ++j) {
        // Loop interno pelas linhas (i)
        for (int i = 0; i < N; ++i) {
            // resultado[i] += matriz[i][j] * vetor[j];
            resultado[i] += matriz[i][j] * vetor[j];
        }
    }
}

int main() {
    const int N = 1000; // Tamanho da matriz NxN e do vetor

    // Cria a matriz NxN com 1 em cada posição
    std::vector<std::vector<int>> matriz(N, std::vector<int>(N, 1));

    // Cria o vetor de entrada com N=1 em cada posição
    std::vector<int> vetor(N, 1);

    // Resultado final, inicialmente todos zeros
    std::vector<int> resultado(N, 0);

    // Quantidade de threads que queremos
    int numThreads = 4;

    // Início da contagem do tempo
    auto inicio = std::chrono::high_resolution_clock::now();

    // Vetor de threads
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Dividir as colunas entre as threads
    // colunasPorThread = N / numThreads
    int colunasPorThread = N / numThreads;
    int resto = N % numThreads; // resto se não for divisão exata

    int colunaAtual = 0;

    // Criamos as threads
    for (int t = 0; t < numThreads; t++) {
        // Se ainda tiver resto, adiciona +1 coluna para esta thread
        int bloco = colunasPorThread + (t < resto ? 1 : 0);

        int colunaInicio = colunaAtual;
        int colunaFim = colunaInicio + bloco;

        // Atualiza para a próxima
        colunaAtual = colunaFim;

        // Emplace_back: cria a thread no vetor
        // Passamos a função e seus argumentos
        threads.emplace_back(
            multiplicarPorColunaParalelo,
            std::cref(matriz),
            std::cref(vetor),
            std::ref(resultado),
            colunaInicio,
            colunaFim
        );
    }

    // Esperamos todas as threads finalizarem
    for (auto& th : threads) {
        th.join();
    }

    // Fim da contagem do tempo
    auto fim = std::chrono::high_resolution_clock::now();
    // Calcula duração
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    // Imprime o tempo
    std::cout << "Tempo de execucao (otimizado, paralelo por coluna): "
              << duracao.count() << " microsegundos" << std::endl;

    return 0;
}

