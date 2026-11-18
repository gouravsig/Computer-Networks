
#include <stdio.h>
#include <string.h>

void manchester(char data[]) {
    printf("\nPolar Manchester:\n");

    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == '0')
            printf("+1 -> -1\n");
        else
            printf("-1 -> +1\n");
    }
}

void differentialManchester(char data[]) {
    int level = 1;

    printf("\nDifferential Manchester:\n");

    for (int i = 0; data[i] != '\0'; i++) {

        if (data[i] == '0')
            level = -level;

        int first = level;

        level = -level;

        int second = level;

        printf("%d -> %d\n", first, second);
    }
}

int main() {
    char data[100];

    printf("Enter binary data: ");
    scanf("%99s", data);

    manchester(data);
    differentialManchester(data);

    return 0;
}
