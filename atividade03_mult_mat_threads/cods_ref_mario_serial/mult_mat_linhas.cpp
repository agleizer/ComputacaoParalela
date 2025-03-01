#include <iostream>
#include <vector>
#include <chrono>

int main() {
  const int N = 1000;
  std::vector<std::vector<int>> matriz(N, std::vector<int>(N, 1));
  std::vector<int> vetor(N, 1);
  std::vector<int> resultado(N, 0);

  // Multiplicação de Matriz por Vetor (Não Otimizado)
  auto inicio = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      resultado[i] += matriz[i][j] * vetor[j];
    }
  }
  auto fim = std::chrono::high_resolution_clock::now();
  auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);
  std::cout << "Tempo de execução (não otimizado): " << duracao.count() << " microsegundos" << std::endl;

  return 0;
}