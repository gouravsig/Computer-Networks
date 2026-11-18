#include <stdio.h>
#include <string.h>

#define FLAG 'F'
#define ESC  'E'

void characterStuffing(const char data[], char stuffed[]) {
    int j = 0;

    stuffed[j++] = FLAG; // Starting frame delimiter

    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == FLAG || data[i] == ESC) {
            stuffed[j++] = ESC; // Stuff escape character
        }
        stuffed[j++] = data[i];
    }

    stuffed[j++] = FLAG; // Ending frame delimiter
    stuffed[j] = '\0';
}

void characterDestuffing(const char stuffed[], char destuffed[]) {
    int len = strlen(stuffed);
    int j = 0;

    // Skip starting FLAG (index 0) and ending FLAG (index len - 1)
    for (int i = 1; i < len - 1; i++) {
        if (stuffed[i] == ESC) {
            i++; // Skip the stuffed ESC character
        }
        destuffed[j++] = stuffed[i];
    }

    destuffed[j] = '\0';
}

int main() {
    char data[100], stuffed[205], destuffed[100];

    printf("Flag character: %c | Escape character: %c\n", FLAG, ESC);
    printf("Enter input data string: ");
    scanf("%99s", data);

    characterStuffing(data, stuffed);
    printf("After Character Stuffing (Transmitted Frame): %s\n", stuffed);

    characterDestuffing(stuffed, destuffed);
    printf("After Character Destuffing (Recovered Data):  %s\n", destuffed);

    return 0;
}