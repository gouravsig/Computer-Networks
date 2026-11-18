#include <stdio.h>

void printBits(int arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d", arr[i]);
    }
    printf("\n");
}

void encode(int a[4], int c[7])
{
    c[0] = a[0] ^ a[1] ^ a[2];
    c[1] = a[1] ^ a[2] ^ a[3];
    c[2] = a[0] ^ a[2] ^ a[3];

    c[3] = a[0];
    c[4] = a[1];
    c[5] = a[2];
    c[6] = a[3];
}

void introduceErrors(int c[7])
{
    int pos;
    printf("Enter bit position to flip (1-7, 0 for none): ");
    scanf("%d", &pos);

    if (pos >= 1 && pos <= 7)
    {
        c[pos - 1] = c[pos - 1] ^ 1;
        printf("Received Codeword  : ");
        printBits(c, 7);
    }
}

void decode(int c[7])
{
    int s0 = c[0] ^ c[3] ^ c[4] ^ c[5];
    int s1 = c[1] ^ c[4] ^ c[5] ^ c[6];
    int s2 = c[2] ^ c[3] ^ c[5] ^ c[6];

    int syndrome = (s0 * 4) + (s1 * 2) + s2;

    int errorIndex[8] = {-1, 2, 1, 6, 0, 3, 4, 5};

    printf("Syndrome (s0 s1 s2): %d%d%d\n", s0, s1, s2);

    if (syndrome == 0)
    {
        printf("No error detected.\n");
    }
    else
    {
        int idx = errorIndex[syndrome];
        printf("Error detected at position %d (index %d).\n", idx + 1, idx);

        c[idx] = c[idx] ^ 1;

        printf("Corrected Codeword : ");
        printBits(c, 7);
    }

    printf("Decoded Dataword   : ");
    printBits(&c[3], 4);
}

int main()
{
    int data[4], codeword[7];

    printf("Enter 4 data bits (space-separated, e.g. 1 0 1 1): ");
    for (int i = 0; i < 4; i++)
    {
        scanf("%d", &data[i]);
    }

    encode(data, codeword);
    printf("Encoded Codeword   : ");
    printBits(codeword, 7);

    introduceErrors(codeword);
    decode(codeword);

    return 0;
}