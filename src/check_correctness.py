import sys
import numpy as np

with open(sys.argv[1]) as file:
    numbers = list(map(int, file.read().split()))

n = numbers[0]
matrix = np.array(numbers[1:], dtype=np.int64).reshape(n, n)
det = np.linalg.det(matrix)

print(f"det: {det}")
