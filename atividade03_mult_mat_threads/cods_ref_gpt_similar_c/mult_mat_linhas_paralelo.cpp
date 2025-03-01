#include <iostream>
#include <thread>
#include <vector>

// Função auxiliar: multiplica uma faixa de linhas de A e B, preenchendo C.
void multiplicarFaixaLinhas(int** A, int linhasA, int colunasA, int** B, int colunasB, int** C, int linhaInicio, int linhaFim) {
    // i percorre apenas o intervalo [linhaInicio, linhaFim).
    for (int i = linhaInicio; i < linhaFim; i++) {
        for (int j = 0; j < colunasB; j++) {
            // Acumulamos o valor de C[i][j].
            for (int k = 0; k < colunasA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Função que multiplica A (linhasA x colunasA) por B (linhasB x colunasB) usando threads.
// A ordem de iteração externa aqui é linha->coluna ("linha-primeiro").
int** multiplicarMatrizLinhaPrimeiro_Paralelo(int** A, int linhasA, int colunasA, int** B, int linhasB, int colunasB, int numThreads) {
    // Validar se colunasA == linhasB (assumimos que o usuário garante isso).

    // 1) Alocar C (linhasA x colunasB) e zerar
    int** C = new int*[linhasA];
    for (int i = 0; i < linhasA; i++) {
        C[i] = new int[colunasB];
        for (int j = 0; j < colunasB; j++) {
            C[i][j] = 0;
        }
    }

    // 2) Criar threads para dividir o trabalho por faixas de linhas
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    // Tamanho do bloco de linhas por thread
    // Exemplo simples: dividir igualmente (ignorando o caso de resto)
    int linhasPorThread = linhasA / numThreads;
    int resto = linhasA % numThreads;

    // Faixa atual de linhas
    int linhaAtual = 0;

    for (int t = 0; t < numThreads; t++) {
        // Se ainda houver resto, dar +1 linha
        int bloco = linhasPorThread + (t < resto ? 1 : 0);
        int linhaInicio = linhaAtual;
        int linhaFim = linhaAtual + bloco;
        linhaAtual = linhaFim;

        // Criar a thread passando a função auxiliar e o intervalo de linhas
        threads.emplace_back(multiplicarFaixaLinhas, A, linhasA, colunasA, B, colunasB, C, linhaInicio, linhaFim);
    }

    // 3) Esperar todas as threads terminarem
    for (auto& th : threads) {
        th.join();
    }

    // 4) Retornar C
    return C;
}

// Função para liberar a memória de uma matriz alocada dinamicamente.
void liberarMatriz(int** matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        delete[] matriz[i];
    }
    delete[] matriz;
}

int main() {
    // Exemplo de dimensões
    int linhasA = 2;
    int colunasA = 3; 
    int linhasB = 3; 
    int colunasB = 2;

    // Alocar A
    int** A = new int*[linhasA];
    for (int i = 0; i < linhasA; i++) {
        A[i] = new int[colunasA];
    }

    // Alocar B
    int** B = new int*[linhasB];
    for (int i = 0; i < linhasB; i++) {
        B[i] = new int[colunasB];
    }

    // Preencher A e B com valores de exemplo
    int valorA = 1;
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasA; j++) {
            A[i][j] = valorA++;
        }
    }
    int valorB = 7;
    for (int i = 0; i < linhasB; i++) {
        for (int j = 0; j < colunasB; j++) {
            B[i][j] = valorB++;
        }
    }

    // Escolher número de threads (por exemplo, 2)
    int numThreads = 2;

    // Multiplicar usando a função paralela
    int** C = multiplicarMatrizLinhaPrimeiro_Paralelo(A, linhasA, colunasA, B, linhasB, colunasB, numThreads);

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
    std::cout << "\nResultado (C = A x B) - Paralelo Linha-Primeiro:\n";
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
