#include <iostream>   // Para std::cout e std::endl
#include <vector>     // Para std::vector
#include <chrono>     // Para medir o tempo (std::chrono)
#include <thread>     // Para usar std::thread
#include <cmath>      // Para operações matemáticas, se necessário (por ex.: std::ceil)

//------------------------------------------------------------------------------
// Função auxiliar: multiplica a parte das linhas de uma matriz por um vetor.
// Cada thread chamará esta função para processar um intervalo de linhas.
//------------------------------------------------------------------------------
void multiplicarPorLinhaParalelo(
    const std::vector<std::vector<int>>& matriz, // (entrada) matriz NxN
    const std::vector<int>& vetor,               // (entrada) vetor de tamanho N
    std::vector<int>& resultado,                 // (saída) resultado parcial
    int linhaInicio,                             // linha inicial a ser processada
    int linhaFim                                 // linha final (exclusivo)
) {
    // Percorre as linhas do intervalo [linhaInicio, linhaFim).
    for (int i = linhaInicio; i < linhaFim; ++i) {
        // Percorre todas as colunas (j) de 0 até N-1 (matriz[i].size() == N).
        for (int j = 0; j < (int)matriz[i].size(); ++j) {
            // Soma parcial no resultado para a linha 'i'.
            // Faz: resultado[i] += matriz[i][j] * vetor[j].
            resultado[i] += matriz[i][j] * vetor[j];
        }
    }
}

int main() {
    const int N = 1000;  // Dimensão da matriz NxN e tamanho do vetor

    // Cria a matriz NxN com valores 1 em cada posição
    std::vector<std::vector<int>> matriz(N, std::vector<int>(N, 1));

    // Cria o vetor de tamanho N também com 1 em cada posição
    std::vector<int> vetor(N, 1);

    // Cria o vetor de resultado, inicialmente zerado
    std::vector<int> resultado(N, 0);

    // Define o número de threads que queremos usar
    int numThreads = 4;

    // Marca o início da medição de tempo (clock)
    auto inicio = std::chrono::high_resolution_clock::now();

    // Cria um vetor de threads, para armazenar as 'numThreads' threads
    std::vector<std::thread> threads;
    // Reservamos espaço no vetor (opcional, mas evita realocações)
    threads.reserve(numThreads);

    // Calculamos quantas linhas cada thread deve processar
    int linhasPorThread = N / numThreads; // divisão inteira
    int resto = N % numThreads;          // se N não for divisível exatamente, sobra 'resto'

    // Variável auxiliar para sabermos onde cada thread começa e termina
    int linhaAtual = 0;

    // Criamos as threads
    for (int t = 0; t < numThreads; t++) {
        // Se ainda houver 'resto', damos +1 linha a esta thread
        int bloco = linhasPorThread + (t < resto ? 1 : 0);

        // O intervalo de linhas para esta thread é [linhaInicio, linhaFim)
        int linhaInicio = linhaAtual;
        int linhaFim = linhaInicio + bloco;

        // Atualizamos linhaAtual para a próxima thread
        linhaAtual = linhaFim;

        // Emplace_back cria a thread diretamente no vetor 'threads'
        // Passamos como parâmetros:
        // - A função multiplicarPorLinhaParalelo
        // - A matriz e vetor (passados por referência constante, std::cref)
        // - O resultado (passado por referência "normal", std::ref, pois vamos escrever nele)
        // - E o intervalo [linhaInicio..linhaFim) que a thread irá processar
        threads.emplace_back(
            multiplicarPorLinhaParalelo,
            std::cref(matriz),
            std::cref(vetor),
            std::ref(resultado),
            linhaInicio,
            linhaFim
        );
    }

    // Agora precisamos "esperar" todas as threads finalizarem antes de continuar
    for (auto& th : threads) {
        th.join(); // join bloqueia até a thread 'th' terminar
    }

    // Marca o fim da medição de tempo
    auto fim = std::chrono::high_resolution_clock::now();

    // Calcula a duração em microsegundos (usando std::chrono::duration_cast)
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    // Imprime o tempo de execução
    std::cout << "Tempo de execucao (nao otimizado, paralelo por linha): "
              << duracao.count() << " microsegundos" << std::endl;

    // Fim do programa
    return 0;
}

