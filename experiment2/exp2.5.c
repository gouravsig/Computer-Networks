#include <stdio.h>

int main() {
    int sources, samples;
    int data[10][10];
    int i, j;

    printf("Enter number of input sources (max 10): ");
    scanf("%d", &sources);

    printf("Enter number of samples per source (max 10): ");
    scanf("%d", &samples);

    if (sources <= 0 || sources > 10 || samples <= 0 || samples > 10) {
        printf("Error: Sources and samples must be between 1 and 10.\n");
        return 1;
    }

    // 1. Read samples for each source (Row-major: Source x Sample)
    for (i = 0; i < sources; i++) {
        printf("\nEnter %d samples from Source %d: ", samples, i + 1);
        for (j = 0; j < samples; j++) {
            scanf("%d", &data[i][j]);
        }
    }

    // 2. Display the stored input matrix
    printf("\nInput Data:\n");
    for (i = 0; i < sources; i++) {
        printf("Source %d :", i + 1);
        for (j = 0; j < samples; j++) {
            printf(" %3d", data[i][j]);
        }
        printf("\n");
    }

    // 3. Interleave into Synchronous TDM frames (Column-major: Frame x Source)
    printf("\nSynchronous TDM Output:\n");
    for (j = 0; j < samples; j++) {
        printf("Frame %d : ", j + 1);
        for (i = 0; i < sources; i++) {
            printf("[S%d: %d] ", i + 1, data[i][j]);
        }
        printf("\n");
    }

    return 0;
}