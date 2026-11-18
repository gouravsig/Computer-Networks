
#include <stdio.h>
#include <string.h>

void nrzL(char data[]) {
    printf("\nPolar NRZ-L:\n");

    //'\0' this represents null character
    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == '1')
            printf("-V ");
        else
            printf("+V ");
    }
}

void nrzI(char data[]) {
    int level = 1;

    printf("\n\nPolar NRZ-I:\n");

    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == '1')
            level = -level;

        if (level == 1)
            printf("+V ");
        else
            printf("-V ");
    }
}

int main() {
    char data[100];

    printf("Enter binary data: ");
    scanf("%99s", data);

    nrzL(data);
    nrzI(data);
    
    printf("\n");

    return 0;
}
