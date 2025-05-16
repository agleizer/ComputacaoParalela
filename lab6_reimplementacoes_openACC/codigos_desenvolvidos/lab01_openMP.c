/*
Universidade Presbiteriana Mackenzie
Computação Paralela - Turma 05P11

Alan Meniuk Gleizer
RA 10416804
14/03/2025
*/

/*
Lab 1 - Programação - Integral Regra do Trapézio (em aula)

Faça o código da integral pela regra do trapézio mostrado nos slides de Introdução ao OpenMP (04 - Programacao de Memoria Compartilhada com OpenMP - ch5), na página 24.
Implemente as diversas melhorias no seu código, até chegar na página 38 (com **reduction** )

Submeta o seu código, com o seu nome no início do código em comentário.
Submeta prints da execução do código.

*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

/*
PARA COMPILAR
gcc -g -Wall -fopenmp lab01_melhorado.c -o lab01_melhorado -lm
*/

/*
OBS. Segui as orientações dos slides, mas as duas paralelizações (em main E em trap) me parecem redundantes
Faznedo alguns testes, fica claro que apenas uma é necessária!
Entendo que utilizamos ambas as abordagens apenas para aprender as diferentes opções, então opttei por deixa-las aqui.
O código será executado com a paralelização apenas em trap....
*/

double f(double a)
{
    return sin(a);
}

double Local_trap(double a, double b, int n)
{
    double h = (b - a) / n;
    double approx = (f(a) + f(b)) / 2.0;
    int i;

    // aqui estamos paralelizando o for loop
    // uso de reduction para cada thread manter uma cópia privada de approx
    // uso de reduction NO LUGAR DE CRITICAL
    #pragma omp parallel for reduction(+ : approx)
    for (i = 1; i <= n - 1; i++)
        approx += f(a + i * h);

    return h * approx;
}

int main(int argc, char *argv[])
{
    double global_result = 0.0; /* Store result in global_result */
    double a, b;                /* Left and right endpoints */
    int n;                      /* Total number of trapezoids */
    int thread_count;

    thread_count = strtol(argv[1], NULL, 10);
    printf("Enter a, b, and n\n");
    scanf("%lf %lf %d", &a, &b, &n);

    // aqui estamos criando um team para executar o += no global_result
    // uso de reduction para cada thread manter uma cópia privada de global_result.
    // uso de reduction NO LUGAR DE CRITICAL
    // #pragma omp parallel num_threads(thread_count) reduction(+ : global_result)
    global_result += Local_trap(a, b, n);

    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.14e\n", a, b, global_result);
    return 0;
} /* main */
