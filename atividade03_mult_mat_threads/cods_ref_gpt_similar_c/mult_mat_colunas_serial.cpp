#include <iostream>

// Similar à função anterior, porém o loop percorre primeiro as colunas, depois as linhas.
int** multiplicarMatrizColunaPrimeiro(int** A, int linhasA, int colunasA, int** B, int linhasB, int colunasB) {
    // 1) Alocar e inicializar C (linhasA x colunasB).
    int** C = new int*[linhasA];
    for (int i = 0; i < linhasA; i++) {
        C[i] = new int[colunasB];
        for (int j = 0; j < colunasB; j++) {
            C[i][j] = 0;
        }
    }

    // 2) Multiplicação "coluna-primeiro":
    //    O loop externo percorre as colunas (j), depois as linhas (i).
    for (int j = 0; j < colunasB; j++) {
        for (int i = 0; i < linhasA; i++) {
            // Aqui, para cada posição (i, j) em C, iteramos sobre k para fazer a soma parcial.
            for (int k = 0; k < colunasA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}

// Reutilizamos a função de liberar memória:
void liberarMatriz2(int** matriz, int linhas) {
    for (int i = 0; i < linhas; i++) {
        delete[] matriz[i];
    }
    delete[] matriz;
}

int main() {
    // Exemplo de dimensões (mesmos valores da versão anterior).
    int linhasA = 2;
    int colunasA = 3;
    int linhasB = 3;
    int colunasB = 2;

    // 1) Alocar A e B.
    int** A = new int*[linhasA];
    for (int i = 0; i < linhasA; i++) {
        A[i] = new int[colunasA];
    }
    int** B = new int*[linhasB];
    for (int i = 0; i < linhasB; i++) {
        B[i] = new int[colunasB];
    }

    // 2) Preencher A e B com valores de exemplo.
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

    // 3) Multiplicar A e B com a ordem de loop "coluna-primeiro".
    int** C = multiplicarMatrizColunaPrimeiro(A, linhasA, colunasA, B, linhasB, colunasB);

    // 4) Exibir A, B e C.
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

    // 5) Liberar memória.
    liberarMatriz2(A, linhasA);
    liberarMatriz2(B, linhasB);
    liberarMatriz2(C, linhasA);

    return 0;
}
