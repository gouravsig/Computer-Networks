#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void goBackN(int total_frames, int window_size) {
    int i = 1; // Starting frame of the current sliding window

    printf("\n--- Go-Back-N ARQ Simulation ---\n\n");

    while (i <= total_frames) {
        int ack_count = 0;

        // 1. Transmit all frames within the current sliding window
        for (int j = i; j < i + window_size && j <= total_frames; j++) {
            printf("Sender: Sent Frame %d\n", j);
        }

        // 2. Check acknowledgments for each sent frame in order
        for (int j = i; j < i + window_size && j <= total_frames; j++) {
            int ack = rand() % 2; // 1 = ACK received, 0 = Frame/ACK lost

            if (ack == 1) {
                printf("Receiver: ACK received for Frame %d\n", j);
                ack_count++;
            } else {
                printf("Timeout! Frame %d lost. Retransmitting from Frame %d...\n", j, j);
                break; // Discard remaining frames in window and go back to frame j
            }
        }

        printf("\n");
        i += ack_count; // Slide window forward by number of consecutive ACKs
    }

    printf("All %d frames transmitted and acknowledged successfully!\n", total_frames);
}

int main() {
    int total_frames, window_size;

    srand(time(NULL)); // Seed random number generator for transmission outcomes

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