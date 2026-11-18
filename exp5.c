#include <stdio.h>

void printBits(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void encode(int a[4], int c[7]) {
    c[0] = a[0] ^ a[1] ^ a[2]; // r0
    c[1] = a[1] ^ a[2] ^ a[3]; // r1
    c[2] = a[0] ^ a[1] ^ a[3]; // r2
    c[3] = a[0];
    c[4] = a[1];
    c[5] = a[2];
    c[6] = a[3];
}

void introduceError(int c[7]) {
    int pos;
    printf("Enter bit position to flip (1-7, 0 for none): ");
    scanf("%d", &pos);
    if (pos >= 1 && pos <= 7)
        c[pos - 1] ^= 1;
}

void decode(int c[7]) {
    int s0 = c[3] ^ c[4] ^ c[5] ^ c[0];
    int s1 = c[4] ^ c[5] ^ c[6] ^ c[1];
    int s2 = c[3] ^ c[4] ^ c[6] ^ c[2];

    printf("Syndrome (s2 s1 s0): %d%d%d\n", s2, s1, s0);

    int map[8] = {-1, 0, 1, 5, 2, 3, 6, 4};
    int s = (s2 * 4) + (s1 * 2) + s0;

    if (s == 0) {
        printf("No error detected.\n");
    } else {
        int idx = map[s];
        printf("Error at position %d. Correcting...\n", idx + 1);
        c[idx] ^= 1;
    }
}

int main() {
    int data[4], code[7];

    printf("Enter 4-bit dataword (a0 a1 a2 a3): ");
    for (int i = 0; i < 4; i++)
        scanf("%d", &data[i]);

    encode(data, code);
    printf("Encoded codeword:   ");
    printBits(code, 7);

    introduceError(code);
    printf("Received codeword:  ");
    printBits(code, 7);

    decode(code);
    printf("Corrected codeword: ");
    printBits(code, 7);

    printf("Decoded dataword:   %d %d %d %d\n", code[3], code[4], code[5], code[6]);

    return 0;
}