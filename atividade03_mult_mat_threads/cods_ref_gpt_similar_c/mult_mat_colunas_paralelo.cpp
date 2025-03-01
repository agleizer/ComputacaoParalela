#include <iostream>
#include <thread>
#include <vector>

// Função auxiliar: multiplica uma faixa de colunas de B, preenchendo C.
void multiplicarFaixaColunas(int** A, int linhasA, int colunasA, int** B, int colunasB, int** C, int colunaInicio, int colunaFim) {
    // Percorremos as colunas no intervalo [colunaInicio, colunaFim).
    for (int j = colunaInicio; j < colunaFim; j++) {
        for (int i = 0; i < linhasA; i++) {
            for (int k = 0; k < colunasA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Função que multiplica A (linhasA x colunasA) por B (linhasB x colunasB) usando threads.
// A ordem de iteração externa aqui é coluna->linha ("coluna-primeiro").
int** multiplicarMatrizColunaPrimeiro_Paralelo(int** A, int linhasA, int colunasA, int** B, int linhasB, int colunasB, int numThreads) {
    // Validar se colunasA == linhasB.

    // 1) Alocar C (linhasA x colunasB) e zerar.
    int** C = new int*[linhasA];
    for (int i = 0; i < linhasA; i++) {
        C[i] = new int[colunasB];
        for (int j = 0; j < colunasB; j++) {
            C[i][j] = 0;
        }
    }

    // 2) Criar threads para dividir o trabalho por faixas de colunas
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    int colunasPorThread = colunasB / numThreads;
    int resto = colunasB % numThreads;

    int colunaAtual = 0;

    for (int t = 0; t < numThreads; t++) {
        // Se ainda houver resto, dar +1 coluna para este thread
        int bloco = colunasPorThread + (t < resto ? 1 : 0);
        int colunaInicio = colunaAtual;
        int colunaFim = colunaAtual + bloco;
        colunaAtual = colunaFim;

        // Criar a thread para processar [colunaInicio, colunaFim)
        threads.emplace_back(multiplicarFaixaColunas,
                             A, linhasA, colunasA,
                             B, colunasB,
                             C,
                             colunaInicio, colunaFim);
    }

    // 3) Esperar as threads
    for (auto& th : threads) {
        th.join();
    }

    // 4) Retornar C
    return C;
}

// Reaproveitamos a função para liberar memória.
void liberarMatriz2(int** matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        delete[] matriz[i];
    }
    delete[] matriz;
}

int main() {
    // Dimensões iguais para comparação com a versão linha-primeiro
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

    // Preencher A e B
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

    // Número de threads
    int numThreads = 2;

    // Multiplicar usando a função paralela "coluna-primeiro"
    int** C = multiplicarMatrizColunaPrimeiro_Paralelo(A, linhasA, colunasA, B, linhasB, colunasB, numThreads);

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
    std::cout << "\nResultado (C = A x B) - Paralelo Coluna-Primeiro:\n";
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasB; j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // Liberar memória
    liberarMatriz2(A, linhasA);
    liberarMatriz2(B, linhasB);
    liberarMatriz2(C, linhasA);

    return 0;
}
