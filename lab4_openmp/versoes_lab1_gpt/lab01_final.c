#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Função que será integrada (pode trocar por outra, se quiser).
double f(double x)
{
    return sin(x);
}

/*
 * Calcula a integral de f(x) em [a, b] usando a Regra do Trapézio,
 * dividindo em n trapézios e paralelizando com OpenMP.
 */
double Trap(double a, double b, int n, int thread_count)
{
    double h = (b - a) / n;
    // Iniciamos approx com a média dos valores nas fronteiras
    double approx = (f(a) + f(b)) / 2.0;

// Paralelizamos o laço dos pontos internos usando "parallel for + reduction"
#pragma omp parallel for num_threads(thread_count) \
    reduction(+ : approx)
    for (int i = 1; i <= n - 1; i++)
    {
        approx += f(a + i * h);
    }

    // Multiplica pela largura de cada trapézio
    approx *= h;
    return approx;
}

int main(int argc, char *argv[])
{
    double a, b, global_result;
    int n, thread_count;

    // Lê o número de threads pela linha de comando
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <número_de_threads>\n", argv[0]);
        return 1;
    }
    thread_count = strtol(argv[1], NULL, 10);

    // Lê os valores de a, b e n do teclado
    printf("Digite os valores de a, b e n: ");
    scanf("%lf %lf %d", &a, &b, &n);

    // Chama a função Trap (já paralelizada)
    global_result = Trap(a, b, n, thread_count);

    // Exibe o resultado
    printf("Com n = %d trapézios, a estimativa da integral de %f a %f = %.14e\n",
           n, a, b, global_result);

    return 0;
}
