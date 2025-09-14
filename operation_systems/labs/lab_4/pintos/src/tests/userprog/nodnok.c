#include <stdio.h>
#include <syscall.h>

int gcd(int a, int b);
int gcd_multiple(int count, int* numbers);
int lcm(int a, int b);
int lcm_multiple(int count, int* numbers);

int main(int argc, const char* argv[]) {
    if (argc < 3 || argc > 7) {
        printf("Usage: %s num1 num2 [num3 ... num6]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int numbers[6];
    int i;
    for (i = 1; i < argc; i++) {
        numbers[i - 1] = atoi(argv[i]);
    }

    int count = argc - 1;
    int gcd_result = gcd_multiple(count, numbers);
    int lcm_result = lcm_multiple(count, numbers);

    printf("GCD: %d\n", gcd_result);
    printf("LCM: %d\n", lcm_result);

    return EXIT_SUCCESS;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int gcd_multiple(int count, int* numbers) {
    int result = numbers[0];
    int i;
    for (i = 1; i < count; i++) {
        result = gcd(result, numbers[i]);
    }
    return result;
}

int lcm(int a, int b) {
    return (a / gcd(a, b)) * b;
}

int lcm_multiple(int count, int* numbers) {
    int result = numbers[0];
    int i;
    for (i = 1; i < count; i++) {
        result = lcm(result, numbers[i]);
    }
    return result;
}
