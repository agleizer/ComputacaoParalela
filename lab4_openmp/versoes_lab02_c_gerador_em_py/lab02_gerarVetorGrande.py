import numpy as np

n = 2**30
filename = "vetor.bin"

# Criar vetor de números de 1 a n
numbers = np.arange(1, n + 1, dtype=np.uint32)  # Garante consistência
np.random.seed(42)  # Garante sempre a mesma ordem
np.random.shuffle(numbers)

# Salvar como binário para facilitar leitura no C
numbers.tofile(filename)