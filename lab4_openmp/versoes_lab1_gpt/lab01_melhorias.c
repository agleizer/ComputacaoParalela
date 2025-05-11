/*
Lab 1 - Programação - Integral Regra do Trapézio (em aula)

Faça o código da integral pela regra do trapézio mostrado nos slides de Introdução ao OpenMP (

04 - Programacao de Memoria Compartilhada com OpenMP - ch5), na página 24.
Implemente as diversas melhorias no seu código, até chegar na página 38 (com **reduction** )

Submeta o seu código, com o seu nome no início do código em comentário.

Submeta prints da execução do código.

PARA COMPILAR
gcc -g -Wall -fopenmp lab01_refslides.c -o melhorado -lm
gcc -g -Wall -fopenmp main.c -o novo -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Função a ser integrada
double f(double x)
{
    return sin(x);
}

/*
 * Função que calcula a integral de f(x) no intervalo [a, b] usando a regra do trapézio.
 * Utilizamos OpenMP para paralelizar o cálculo e melhorar a eficiência.
 *
 * Parâmetros:
 * - a: limite inferior da integral
 * - b: limite superior da integral
 * - n: número de trapézios utilizados na aproximação
 *
 * Retorna:
 * - O valor aproximado da integral.
 */
double Local_trap(double a, double b, int n)
{
    double h, approx;
    int i;

    // Calculamos o tamanho de cada trapézio
    h = (b - a) / n;

    // Inicializamos a soma com os valores das extremidades divididos por 2,
    // pois na regra do trapézio somamos f(a) e f(b) uma única vez
    approx = (f(a) + f(b)) / 2.0;

/*
 * Agora utilizamos a diretiva "parallel for" para paralelizar o loop.
 * A cláusula "reduction(+: approx)" garante que as threads somem os resultados
 * de maneira eficiente, evitando sincronizações manuais e seções críticas.
 */
#pragma omp parallel for reduction(+ : approx)
    for (i = 1; i <= n - 1; i++)
    {
        approx += f(a + i * h); // Soma dos valores da função nos pontos intermediários
    }

    // Multiplicamos pelo tamanho dos trapézios para obter a aproximação final da integral
    return h * approx;
}

int main(int argc, char *argv[])
{
    double a, b, global_result = 0.0;
    int n, thread_count;

    // Verificação de entrada correta dos argumentos
    if (argc != 2)
    {
        printf("Uso: %s <número de threads>\n", argv[0]);
        return 1;
    }

    // Obtém o número de threads da linha de comando
    thread_count = strtol(argv[1], NULL, 10);

    // Solicita ao usuário os valores dos limites e o número de trapézios
    printf("Digite os valores de a, b e n: ");
    scanf("%lf %lf %d", &a, &b, &n);

/*
 * Aplicamos a diretiva "parallel num_threads(thread_count)" para dividir o trabalho entre as threads.
 * Cada thread executa "Local_trap", e os resultados são acumulados em "global_result"
 * usando "reduction(+: global_result)", evitando concorrência e a necessidade de sincronização manual.
 */
#pragma omp parallel num_threads(thread_count) reduction(+ : global_result)
    global_result += Local_trap(a, b, n);

    // Exibe o resultado final da integral aproximada
    printf("Com n = %d trapézios, a estimativa da integral de %f a %f = %.14e\n", n, a, b, global_result);

    return 0;
}
