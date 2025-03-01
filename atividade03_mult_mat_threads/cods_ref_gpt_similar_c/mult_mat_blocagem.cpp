#include <iostream>   // Para std::cout, std::endl
#include <thread>     // Para std::thread
#include <vector>     // Para std::vector (armazenar threads)
#include <algorithm>  // Para std::min

// Função que aloca dinamicamente uma matriz de int (linhas x colunas).
int** alocarMatriz(int linhas, int colunas) {
    int** M = new int*[linhas];
    for (int i = 0; i < linhas; i++) {
        M[i] = new int[colunas];
    }
    return M;
}

// Função que libera a memória de uma matriz alocada dinamicamente.
void liberarMatriz(int** M, int linhas) {
    for (int i = 0; i < linhas; i++) {
        delete[] M[i];
    }
    delete[] M;
}

//------------------------------------------------------------------------------
// Função auxiliar (chamada por cada thread):
// Processa uma FAIXA de blocos na dimensão "iBlock" (linhas do resultado).
//------------------------------------------------------------------------------
void multiplicarFaixaDeBlocos(
    int** A, int linhasA, int colunasA,
    int** B, /*linhasB=*/int, int colunasB,
    int** C,
    int blockSize,
    int iBlockInicio, int iBlockFim
) {
    // Número de blocos na dimensão i (linhasA) e j (colunasB) e k (colunasA).
    int numBlocksI = (linhasA + blockSize - 1) / blockSize;
    int numBlocksJ = (colunasB + blockSize - 1) / blockSize;
    int numBlocksK = (colunasA + blockSize - 1) / blockSize;

    // Percorre blocos de i (mas só a faixa [iBlockInicio..iBlockFim) dessa thread).
    for (int bi = iBlockInicio; bi < iBlockFim; bi++) {
        // Determina os limites (início e fim) reais no eixo i.
        int iInicio = bi * blockSize;
        int iFim = std::min(iInicio + blockSize, linhasA);

        // Para cada bloco de j (todas as colunas).
        for (int bj = 0; bj < numBlocksJ; bj++) {
            int jInicio = bj * blockSize;
            int jFim = std::min(jInicio + blockSize, colunasB);

            // Para cada bloco de k (parte intermediária).
            for (int bk = 0; bk < numBlocksK; bk++) {
                int kInicio = bk * blockSize;
                int kFim = std::min(kInicio + blockSize, colunasA);

                // Agora faz a soma parcial para cada (i, j) dentro do bloco.
                for (int i = iInicio; i < iFim; i++) {
                    for (int j = jInicio; j < jFim; j++) {
                        int soma = 0;
                        // Soma A[i][k] * B[k][j] para k no [kInicio..kFim).
                        for (int k = kInicio; k < kFim; k++) {
                            soma += A[i][k] * B[k][j];
                        }
                        // Acumula no elemento de C.
                        C[i][j] += soma;
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Função principal de multiplicação com blocagem + paralelismo:
// A: (linhasA x colunasA), B: (colunasA x colunasB) -> C: (linhasA x colunasB).
//------------------------------------------------------------------------------
int** multiplicarMatrizBlocosParalelo(
    int** A, int linhasA, int colunasA,
    int** B, int linhasB, int colunasB,
    int blockSize,
    int numThreads
) {
    // Verificamos se realmente colunasA == linhasB, senão não multiplica.
    if (colunasA != linhasB) {
        std::cerr << "Dimensões incompatíveis para multiplicação!\n";
        return nullptr; // ou poderíamos lançar exceção ou tratar de outra forma
    }

    // 1) Alocar a matriz C e inicializar com 0
    int** C = alocarMatriz(linhasA, colunasB);
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasB; j++) {
            C[i][j] = 0;
        }
    }

    // 2) Calcular quantos blocos existem no eixo i (linhas de C)
    int numBlocksI = (linhasA + blockSize - 1) / blockSize;

    // 3) Criar as threads, cada uma processa uma faixa de blocos iBlock
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Descobrir quantos blocos cada thread vai processar
    int blocosPorThread = numBlocksI / numThreads;  // divisão inteira
    int resto = numBlocksI % numThreads;            // resto da divisão

    int blocoAtual = 0;
    for (int t = 0; t < numThreads; t++) {
        // Se ainda houver resto, dar +1 bloco a esta thread
        int qtdBlocos = blocosPorThread + ((t < resto) ? 1 : 0);

        int iBlockInicio = blocoAtual;
        int iBlockFim = iBlockInicio + qtdBlocos; // exclusivo
        blocoAtual = iBlockFim;

        // Criar a thread
        threads.emplace_back(
            multiplicarFaixaDeBlocos,
            A, linhasA, colunasA,
            B, linhasB, colunasB,
            C,
            blockSize,
            iBlockInicio, iBlockFim
        );
    }

    // 4) Esperar as threads terminarem
    for (auto &th : threads) {
        th.join();
    }

    // Retornar C
    return C;
}

//------------------------------------------------------------------------------
// Programa de exemplo
//------------------------------------------------------------------------------
int main() {
    // Exemplo pequeno: A (2x3) e B (3x2).
    // Em casos maiores, a blocagem fica bem mais importante.
    int linhasA = 2;
    int colunasA = 3;

    int linhasB = 3;
    int colunasB = 2;

    // Alocar matrizes A e B
    int** A = alocarMatriz(linhasA, colunasA);
    int** B = alocarMatriz(linhasB, colunasB);

    // Preencher A com valores simples
    // A = [ [1, 2, 3],
    //       [4, 5, 6] ]
    int valor = 1;
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasA; j++) {
            A[i][j] = valor++;
        }
    }

    // Preencher B com valores simples
    // B = [ [7,  8],
    //       [9, 10],
    //       [11,12] ]
    valor = 7;
    for (int i = 0; i < linhasB; i++) {
        for (int j = 0; j < colunasB; j++) {
            B[i][j] = valor++;
        }
    }

    // Definir blockSize e número de threads
    // Em um caso real, blockSize deve ser testado (16, 32, 64, etc.)
    int blockSize = 2;  
    int numThreads = 2;

    // Multiplicar A e B usando blocagem + threads
    int** C = multiplicarMatrizBlocosParalelo(
        A, linhasA, colunasA,
        B, linhasB, colunasB,
        blockSize,
        numThreads
    );

    // Se C == nullptr, houve erro de dimensões
    if (!C) {
        liberarMatriz(A, linhasA);
        liberarMatriz(B, linhasB);
        return 1;
    }

    // Exibir A
    std::cout << "Matriz A:\n";
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasA; j++) {
            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // Exibir B
    std::cout << "\nMatriz B:\n";
    for (int i = 0; i < linhasB; i++) {
        for (int j = 0; j < colunasB; j++) {
            std::cout << B[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // Exibir C
    std::cout << "\nResultado C = A x B (Blocagem + Threads):\n";
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasB; j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // Liberar memória
    liberarMatriz(A, linhasA);
    liberarMatriz(B, linhasB);
    liberarMatriz(C, linhasA);

    return 0;
}
