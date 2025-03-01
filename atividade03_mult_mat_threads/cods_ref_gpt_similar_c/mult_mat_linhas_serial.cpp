#include <iostream>

// Função que multiplica duas matrizes A (linhasA x colunasA) e B (linhasB x colunasB)
// Retorna a matriz resultante C (linhasA x colunasB).
// Observação: assumimos colunasA == linhasB para a multiplicação ser válida.
int** multiplicarMatrizLinhaPrimeiro(int** A, int linhasA, int colunasA, int** B, int linhasB, int colunasB) {
    // 1) Alocar dinamicamente C (linhasA x colunasB).
    //    Cada linha de C será um array de 'colunasB' inteiros.
    int** C = new int*[linhasA];
    for (int i = 0; i < linhasA; i++) {
        C[i] = new int[colunasB];
        // Inicializa cada elemento de C[i] com 0.
        for (int j = 0; j < colunasB; j++) {
            C[i][j] = 0;
        }
    }

    // 2) Realizar a multiplicação de matrizes no estilo "linha-primeiro".
    //    Ou seja: o loop externo percorre as linhas de A, depois as colunas de B.
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasB; j++) {
            // i e j definem a posição (linha, coluna) em C.
            // Precisamos somar A[i][k] * B[k][j] para k em [0..colunasA-1].
            for (int k = 0; k < colunasA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // 3) Retornar o ponteiro para a matriz resultante.
    return C;
}

// Função auxiliar para liberar memória de uma matriz alocada dinamicamente.
void liberarMatriz(int** matriz, int linhas) {
    // Deletar cada array de colunas.
    for (int i = 0; i < linhas; i++) {
        delete[] matriz[i];
    }
    // Deletar o array de ponteiros de linha.
    delete[] matriz;
}

int main() {
    // Exemplo de dimensões.
    // linhasA x colunasA para a matriz A
    // linhasB x colunasB para a matriz B
    // Observação: colunasA deve ser igual a linhasB para multiplicar.
    int linhasA = 2;
    int colunasA = 3;
    int linhasB = 3;
    int colunasB = 2;

    // 1) Alocar dinamicamente a matriz A (linhasA x colunasA).
    int** A = new int*[linhasA];
    for (int i = 0; i < linhasA; i++) {
        A[i] = new int[colunasA];
    }

    // 2) Alocar dinamicamente a matriz B (linhasB x colunasB).
    int** B = new int*[linhasB];
    for (int i = 0; i < linhasB; i++) {
        B[i] = new int[colunasB];
    }

    // 3) Preencher A com valores de exemplo.
    //    Aqui, definimos manualmente (ou poderíamos ler de arquivo/usuário).
    //    Exemplo: A = [ [1, 2, 3],
    //                   [4, 5, 6] ]
    int valorA = 1;
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasA; j++) {
            A[i][j] = valorA++;
        }
    }

    // 4) Preencher B com valores de exemplo.
    //    Exemplo: B = [ [7,  8],
    //                   [9, 10],
    //                   [11,12] ]
    int valorB = 7;
    for (int i = 0; i < linhasB; i++) {
        for (int j = 0; j < colunasB; j++) {
            B[i][j] = valorB++;
        }
    }

    // 5) Multiplicar A e B -> obter C.
    int** C = multiplicarMatrizLinhaPrimeiro(A, linhasA, colunasA, B, linhasB, colunasB);

    // 6) Exibir as matrizes A, B e o resultado C.
    std::cout << "Matriz A (" << linhasA << " x " << colunasA << "):\n";
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasA; j++) {
            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "\nMatriz B (" << linhasB << " x " << colunasB << "):\n";
    for (int i = 0; i < linhasB; i++) {
        for (int j = 0; j < colunasB; j++) {
            std::cout << B[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "\nResultado C = A x B (" << linhasA << " x " << colunasB << "):\n";
    for (int i = 0; i < linhasA; i++) {
        for (int j = 0; j < colunasB; j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // 7) Liberar a memória alocada para A, B e C.
    liberarMatriz(A, linhasA);
    liberarMatriz(B, linhasB);
    liberarMatriz(C, linhasA);

    return 0;
}
