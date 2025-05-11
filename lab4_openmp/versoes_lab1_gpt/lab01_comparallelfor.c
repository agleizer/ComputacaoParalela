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
 * - thread_count: número de threads utilizadas na paralelização
 *
 * Retorna:
 * - O valor aproximado da integral.
 */
double Trap(double a, double b, int n, int thread_count)
{
    double h, approx = 0.0;
    int i;

    // Calculamos o tamanho de cada trapézio
    h = (b - a) / (double)n;

    // Inicializamos a soma com os valores das extremidades divididos por 2,
    // pois na regra do trapézio somamos f(a) e f(b) uma única vez
    approx = (f(a) + f(b)) / 2.0;

/*
 * Agora utilizamos a diretiva "parallel for" para paralelizar o loop.
 * A cláusula "reduction(+: approx)" garante que as threads somem os resultados
 * de maneira eficiente, evitando sincronizações manuais e seções críticas.
 * Além disso, usamos "num_threads(thread_count)" para definir o número de threads.
 */
#pragma omp parallel for num_threads(thread_count) reduction(+ : approx)
    for (i = 1; i < n; i++)
    {                           // Ajuste do limite superior
        approx += f(a + i * h); // Soma dos valores da função nos pontos intermediários
    }

    // Multiplicamos pelo tamanho dos trapézios para obter a aproximação final da integral
    approx *= h;
    return approx;
}

int main(int argc, char *argv[])
{
    double a, b, global_result;
    int n, thread_count;

    // Verificação de entrada correta dos argumentos
    if (argc != 2)
    {
        printf("Uso: %s <número de threads>\n", argv[0]);
        return 1;
    }

    // Obtém o número de threads da linha de comando
    thread_count = strtol(argv[1], NULL, 10);
    omp_set_num_threads(thread_count);

    // Solicita ao usuário os valores dos limites e o número de trapézios
    printf("Digite os valores de a, b e n: ");
    scanf("%lf %lf %d", &a, &b, &n);

    /*
     * Chamamos a função Trap() com o número correto de threads,
     * garantindo que a integral seja calculada paralelamente e corretamente acumulada.
     */
    global_result = Trap(a, b, n, thread_count);

    // Exibe o resultado final da integral aproximada
    printf("Com n = %d trapézios, a estimativa da integral de %f a %f = %.14e\n", n, a, b, global_result);

    return 0;
}
