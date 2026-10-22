import random

n = 13
filename = f"examples/matrix-{n}.txt"

random.seed(52)
with open(filename, "w") as file:
    file.write(f"{n}\n")
    for row in range(n):
        file.write(" ".join(str(random.randint(-1, 1)) for column in range(n)) + "\n")
