#include <stdio.h>
#include <stdlib.h>

#define A 1103515245
#define C 12345
#define MOD (1U << 31)

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s lower upper N S\n", argv[0]);
        return 1;
    }

    int lower = atoi(argv[1]);
    int upper = atoi(argv[2]);
    int N = atoi(argv[3]);
    int S = atoi(argv[4]);

    if (lower > upper) {
        fprintf(stderr, "Error: lower must be <= upper\n");
        return 1;
    }

    if (N < 0) {
        fprintf(stderr, "Error: N must be non-negative\n");
        return 1;
    }

    if (S < 0) {
        fprintf(stderr, "Error: S must be non-negative\n");
        return 1;
    }

    unsigned int seed = S;
    int range = upper - lower + 1;

    for (int i = 0; i < N; i++) {
        seed = (A * seed + C) % MOD;
        int value = lower + (seed % range);
        printf("%d\n", value);
    }

    return 0;
}