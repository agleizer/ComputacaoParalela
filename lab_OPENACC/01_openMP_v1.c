#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

/*
OBS: implementacao da versao simples vista em teoria! cada processador recebe 1 elemento
TODO: implementar versao com "blocos" (ver merge_slip low e high no enunciado)
*/

/*
gcc -fopenmp -O3 -Wall 01_openMP_v1.c -o 01_openMP_v1
./ 01_openMP_v1 1000 16
*/

// func para trocar dois inteiros
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
odd even sort
n = tamanho do vetor, p = numero de threads (não está sendo usando dentro da funcao!!)
*/
void odd_even_sort_omp(int *data, int n, int p) {
    // loop para cada fase (n fases pois no odd even, o algoritmo paralelo leva O(n) * n processadores)
    for (int fase = 0; fase < n; ++fase) {
        if (fase % 2 == 0) {
            // fase par
            #pragma omp parallel for
            for (int i = 0; i + 1 < n; i += 2) {
                if (data[i] > data[i+1]) {
                    swap(&data[i], &data[i+1]);
                }
            }
        } else {
            // fase impar
            #pragma omp parallel for
            for (int i = 1; i + 1 < n; i += 2) {
                if (data[i] > data[i+1]) {
                    swap(&data[i], &data[i+1]);
                }
            }
        }
        // pragma omp parallel for não precisa de barrier
    }
}

int main(int argc, char **argv) {

    // leitura de args
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tam vetor> <num threads>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]); // num de elementos
    int p = atoi(argv[2]); // num de threads

    omp_set_num_threads(p);

    // seed para manter elementos
    srand(10416804);

    // cria vetor e popula com numeros aleatorios
    int *data = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) {
        data[i] = rand() % 1000 + 1; // aleatorio entre 1 e 1000
    }

    // chamada do odd even com medicao de tempo
    double t0 = omp_get_wtime();
    odd_even_sort_omp(data, n, p);
    double t1 = omp_get_wtime();

    printf("OpenMP: n = %d, threads = %d -> %f s\n", n, p, t1 - t0);

    free(data);
    return 0;
}
