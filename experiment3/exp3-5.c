#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void goBackN(int total_frames, int window_size) {
    int i = 1; 

    printf("\n--- Go-Back-N ARQ Simulation ---\n\n");

    while (i <= total_frames) {
        int ack_count = 0;

        for (int j = i; j < i + window_size && j <= total_frames; j++) {
            printf("Sender: Sent Frame %d\n", j);
        }

        for (int j = i; j < i + window_size && j <= total_frames; j++) {
            int ack = rand() % 2; 
 
            if (ack == 1) {
                printf("Receiver: ACK received for Frame %d\n", j);
                ack_count++;
            } else {
                printf("Timeout! Frame %d lost. Retransmitting from Frame %d...\n", j, j);
                break; 
            }
        }

        printf("\n");
        i += ack_count; 
    }

    printf("All %d frames transmitted and acknowledged successfully!\n", total_frames);
}

int main() {
    int total_frames, window_size;

    srand(time(NULL)); 

    printf("Enter total number of frames: ");
    scanf("%d", &total_frames);

    printf("Enter sliding window size: ");
    scanf("%d", &window_size);

    if (total_frames <= 0 || window_size <= 0) {
        printf("Number of frames and window size must be greater than 0.\n");
        return 1;
    }

    goBackN(total_frames, window_size);

    return 0;
}