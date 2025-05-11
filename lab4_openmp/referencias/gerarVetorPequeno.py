import random

def main():
    n = 2**30

    filename = f"vetor_{n}.txt"

    numbers = list(range(1, n + 1))

    random.shuffle(numbers)

    with open(filename, "w") as f:
        for num in numbers:
            f.write(f"{num}\n")

if __name__ == "__main__":
    main()