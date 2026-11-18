
#include <stdio.h>
#include <string.h>

void findXor(char dividend[], char divisor[], int pos, int m) {
    dividend[pos] = '0';

    for (int i = 1; i < m; i++) {
        dividend[pos + i] =
            (dividend[pos + i] == divisor[i]) ? '0' : '1';
    }
}

void mod2div(char dividend[], char divisor[], char remainder[]) {
    int n = strlen(dividend);
    int m = strlen(divisor);

    for (int i = 0; i <= n - m; i++) {
        if (dividend[i] == '1') {
            findXor(dividend, divisor, i, m);
        }
    }

    strcpy(remainder, dividend + n - m + 1);
}

void encodeData(char data[], char divisor[], char encoded[]) {
    int d = strlen(data);
    int m = strlen(divisor);

    char dividend[200], remainder[100];

    strcpy(dividend, data);

    for (int i = 0; i < m - 1; i++)
        dividend[d + i] = '0';

    dividend[d + m - 1] = '\0';

    mod2div(dividend, divisor, remainder);

    strcpy(encoded, data);
    strcat(encoded, remainder);
}

void receiver(char received[], char divisor[]) {
    char remainder[100];

    mod2div(received, divisor, remainder);

    printf("Receiver Remainder: %s\n", remainder);

    if (strspn(remainder, "0") == strlen(remainder))
        printf("No Error Detected\n");
    else
        printf("Error Detected\n");
}

int main() {
    char data[100], divisor[100];
    char encoded[200], received[200];

    printf("Enter data bits: ");
    scanf("%99s", data);

    printf("Enter generator bits: ");
    scanf("%99s", divisor);

    if (strlen(divisor) < 2 ||
        divisor[0] != '1' ||
        strlen(data) + strlen(divisor) > 199) {
        printf("Invalid input!\n");
        return 1;
    }

    encodeData(data, divisor, encoded);

    printf("\nSender Side\n");
    printf("Transmitted Codeword: %s\n", encoded);

    printf("\nReceiver Side\n");
    strcpy(received, encoded);
    receiver(received, divisor);

    printf("\nError Simulation\n");
    strcpy(received, encoded);

    received[0] = (received[0] == '0') ? '1' : '0';

    printf("Corrupted Data: %s\n", received);
    receiver(received, divisor);

    return 0;
}
