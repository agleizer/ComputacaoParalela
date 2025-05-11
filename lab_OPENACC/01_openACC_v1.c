#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <openacc.h>

/*
ASSIM COMO A VERSAO OPENMP
OBS: implementacao da versao simples vista em teoria! cada processador recebe 1 elemento
TODO: implementar versao com "blocos" (ver merge_slip low e high no enunciado)
*/

/*
nvc -acc -Minfo=accel -O3 -Wall 01_openACC_v1.c -o 01_openACC_v1
./01_openACC_v1 500
*/

// func para trocar dois inteiros
// pragma necessario p indicar que a funcao sera chamada dentro de uma regiao paralela da GPU 
#pragma acc routine seq
void swap_int(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
odd even sort
n = tamanho do vetor, p = numero de threads
*/
void odd_even_sort_acc(int *data, int n, int p) {
    // datat copy para enviar todos os dados para a GPU
    #pragma acc data copy(data[0:n])
    {
        // loop para cada fase (n fases pois no odd even, o algoritmo paralelo leva O(n) * n processadores)
        for (int fase = 0; fase < n; ++fase) {
            if (fase % 2 == 0) {
                // fase par
                int i;
                #pragma acc parallel loop
                for (i = 0; i + 1 < n; i += 2) {
                    if (data[i] > data[i+1]) {
                        swap_int(&data[i], &data[i+1]);
                    }
                }
            } else {
                // fase impar
                int i;
                #pragma acc parallel loop
                for (i = 1; i + 1 < n; i += 2) {
                    if (data[i] > data[i+1]) {
                        swap_int(&data[i], &data[i+1]);
                    }
                }
            }
            // diferente do omp paralel for, acc parallel loop precisa de barreira!
            #pragma acc wait
        }
    }
}

int main(int argc, char **argv) {

    // leitura de args
    // nessa versao nao eh possivel controlar numero de "threads"...
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <tam vetor>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);  // num de elementos

    // seed para manter elementos
    srand(10416804);

    // cria vetor e popula com numeros aleatorios
    int *data = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) {
        data[i] = rand() % 1000 + 1; // aleatorio entre 1 e 1000
    }

    // chamada do odd even com medicao de tempo (incl tempo de copia de/para GPU)
    struct timeval tv0, tv1;
    gettimeofday(&tv0, NULL);

    odd_even_sort_acc(data, n, /*p=*/0);

    gettimeofday(&tv1, NULL);
    double tempo = (tv1.tv_sec - tv0.tv_sec)
                   + (tv1.tv_usec - tv0.tv_usec) * 1e-6;

    printf("OpenACC: n = %d -> %f s\n", n, tempo);

    free(data);
    return 0;
}
