
#include <stdio.h>
#include <string.h>

void twoB1Q(char data[]) {
    int n = strlen(data);

    if (n % 2 != 0) {
        printf("\nNote: Odd length binary string detected. ");
        printf("Padded with '0' to make length even.\n");

        data[n] = '0';
        data[n + 1] = '\0';
        n++;
    }

    printf("\n[2B1Q (Two Binary, One Quaternary) Encoding]\n");

    printf("Binary Input (Bits): ");

    for (int i = 0; i < n; i += 2)
        printf("%c%c  ", data[i], data[i + 1]);

    printf("\nQuaternary Levels:   ");

    for (int i = 0; i < n; i += 2) {

        if (data[i] == '0' && data[i + 1] == '0')
            printf("-3V  ");

        else if (data[i] == '0' && data[i + 1] == '1')
            printf("-1V  ");

        else if (data[i] == '1' && data[i + 1] == '1')
            printf("+1V  ");

        else
            printf("+3V  ");
    }

    printf("\n");
}

int main() {
    char data[102];

    printf("Enter a binary sequence for 2B1Q encoding: ");
    scanf("%100s", data);

    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] != '0' && data[i] != '1') {
            printf("Invalid binary input!\n");
            return 1;
        }
    }

    twoB1Q(data);

    return 0;
}
