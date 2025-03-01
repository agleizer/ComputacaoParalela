#include <iostream>
#include <vector>
#include <chrono>

int main() {
  const int N = 1000;
  const int BLOCK_SIZE = 16; // Tamanho do bloco
  std::vector<std::vector<int>> matriz(N, std::vector<int>(N, 1));
  std::vector<int> vetor(N, 1);
  std::vector<int> resultado(N, 0);

  // Multiplicação de Matriz por Vetor (Com Blocagem)
  auto inicio = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < N; j += BLOCK_SIZE) {
    for (int i = 0; i < N; ++i) {
      for (int k = j; k < std::min(j + BLOCK_SIZE, N); ++k) {
        resultado[i] += matriz[i][k] * vetor[k];
      }
    }
  }
  auto fim = std::chrono::high_resolution_clock::now();
  auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);
  std::cout << "Tempo de execução (blocagem): " << duracao.count() << " microsegundos" << std::endl;

  return 0;
}
