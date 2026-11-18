#include <stdio.h>
#include <string.h>


void encode2B1Q(char *binaryString)
{
    int length = strlen(binaryString);

    char padded[102];
    strcpy(padded, binaryString);
    if (length % 2 != 0)
    {
        strcat(padded, "0");
        length++;
        printf("Note: Odd length binary string detected. Padded with 0 to make length even.\n");
    }

    int prevLevel = 1;

    printf("\n2B1Q Encoded output:\n");

    for (int i = 0; i < length; i += 2) {
        char firstBit = padded[i];
        char secondBit = padded[i + 1];

        int magnitude = (secondBit == '0') ? 1 : 3;

        int sign;
        if (prevLevel > 0) {
            sign = (firstBit == '0') ? 1 : -1;
        } else {
            sign = (firstBit == '0') ? -1 : 1;
        }

        int currentLevel = sign * magnitude;

        if (currentLevel > 0)
            printf("+%d ", currentLevel);
        else
            printf("%d ", currentLevel);

        prevLevel = currentLevel;
    }

    printf("\n");
}

int main() {
    char data[100];

    printf("Enter binary data: ");
    scanf("%99s", data);

    encode2B1Q(data);

    return 0;
}