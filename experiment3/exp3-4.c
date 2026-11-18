#include <stdio.h>
#include <string.h>

void bitStuffing(const char *input, char *stuffed) {
    int count = 0, j = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '1') {
            count++;
        } else {
            count = 0;
        }

        stuffed[j++] = input[i];

        if (count == 5) {
            stuffed[j++] = '0'; 
            count = 0;          
        }
    }
    stuffed[j] = '\0';
}

void bitDestuffing(const char *stuffed, char *destuffed) {
    int count = 0, j = 0;

    for (int i = 0; stuffed[i] != '\0'; i++) {
        if (stuffed[i] == '1') {
            count++;
        } else {
            count = 0;
        }

        destuffed[j++] = stuffed[i];

        if (count == 5 && stuffed[i + 1] == '0') {
            i++;       
            count = 0; 
        }
    }
    destuffed[j] = '\0';
}

int main() {
    char input[100], stuffed[130], destuffed[100];

    printf("Enter input bit stream (0s and 1s): ");
    scanf("%99s", input);

    bitStuffing(input, stuffed);
    printf("After Bit Stuffing   : %s\n", stuffed);

    bitDestuffing(stuffed, destuffed);
    printf("After Bit Destuffing : %s\n", destuffed);

    return 0;
}