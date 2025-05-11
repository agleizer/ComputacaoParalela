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
#include <thread>

// OBS: código realiza multiplicação de matrizes QUADRADAS de mesmo tamanho para simplificar
// OBS: tam. da matriz precisa ser divisivel pelo numero de threads! caso contrario, descomentar "resto"

// função de multiplicação de um INTERVALO de linhas
// será chamada por cada thread
// parametros: matriz A, matriz B, matriz C (resultado), linhas inicial e final [inicial, final[
// OBS: usamos linhas mas fazendo blocagem na dimensão das COLUNAS dentro do intervalo de linhas.
void multiplicarMatrizesPorBlocagem(const std::vector<std::vector<int>>& A,const std::vector<std::vector<int>>& B,std::vector<std::vector<int>>& C,int linhaInicio,int linhaFim){
    int N = (int)A.size();
    const int BLOCK_SIZE = 16;    // Tamanho do bloco de colunas

    // percorrer as colunas em blocos de tamanho BLOCK_SIZE
    for (int jBlock = 0; jBlock < N; jBlock += BLOCK_SIZE) {
        int jFim = std::min(jBlock + BLOCK_SIZE, N);

        // Para cada linha no INTERVALO [linhaInicio..linhaFim[
        for (int i = linhaInicio; i < linhaFim; i++) {
            // Para cada coluna no bloco [jBlock..jFim)
            for (int j = jBlock; j < jFim; j++) {
                int soma = 0;
                // Soma parcial em k, percorrendo todas as colunas de A (ou linhas de B)
                for (int k = 0; k < N; k++) {
                    soma += A[i][k] * B[k][j];
                }
                C[i][j] = soma;
            }
        }
    }
}

int main() {
    // def tamanho NxN (o mesmo do código anterior)
    const int N = 500;

    // criação de matrizes A e B, ambas NxN com valor 5
    std::vector<std::vector<int>> A(N, std::vector<int>(N, 5));
    std::vector<std::vector<int>> B(N, std::vector<int>(N, 5));

    // criação de C (resultado), NxN inicializada com 0
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    // def numero de threads
    int numThreads = 10;

    // inicio do cronômetro
    auto inicio = std::chrono::high_resolution_clock::now();

    // Vetor para armazenar as threads
    std::vector<std::thread> threads;

    // cálculo de quantas linhas cada thread processa
    int linhasPorThread = N / numThreads;
    int linhaAtual = 0;

    // criar as threads, cada uma processando um intervalo [linhaInicio..linhaFim)
    for (int t = 0; t < numThreads; t++) {
        int bloco = linhasPorThread;
        int linhaInicio = linhaAtual;
        int linhaFim = linhaInicio + bloco;
        linhaAtual = linhaFim;

        threads.push_back(std::thread(multiplicarMatrizesPorBlocagem,std::cref(A),std::cref(B),std::ref(C),linhaInicio,linhaFim));
    }

    // aguardar as threads
    for (auto& th : threads) {
        th.join();
    }

    // parar cronômetro
    auto fim = std::chrono::high_resolution_clock::now();
    // calcular em microsegundos
    auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

    // Imprime um elemento e o tempo de execução
    std::cout << "\nC[0][0] = " << C[0][0] << std::endl;
    std::cout << "Tempo de execucao (blocagem, paralelo): "
              << duracao.count() << " microsegundos\n" << std::endl;

    return 0;
}