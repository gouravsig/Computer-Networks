#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_STATIONS 5
#define MAX_ATTEMPTS 10

int isMediumIdle()
{
    return rand() % 2;
}

int detectCollision()
{
    return rand() % 4 == 0; 
}

void backoff(int attempt)
{
    int k = (attempt < 10) ? attempt : 10;
    int r = rand() % (1 << k);
    printf("    Waiting for %d units before retrying...\n", r);
    sleep(r % 3);
}

void transmitFrame(int stationId, const char* frame)
{
    int attempts = 0;

    printf("\nStation %d attempting to transmit: \"%s\"\n", stationId, frame);

    while (attempts < MAX_ATTEMPTS)
    {
        if (isMediumIdle())
        {
            printf("  Medium is idle. Transmitting...\n");
            sleep(1); 

            if (detectCollision())
            {
                printf("  ** Collision detected! **\n");
                attempts++;
                backoff(attempts);
            }
            else
            {
                printf("  Frame \"%s\" transmitted successfully by Station %d.\n", frame, stationId);
                return;
            }
        }
        else
        {
            printf("  Medium is busy. Sensing again...\n");
            sleep(1);
        }
    }

    printf("  Transmission failed after %d attempts. Giving up.\n", MAX_ATTEMPTS);
}

int main() {
    srand(time(NULL));
    int numStations;
    char frames[MAX_STATIONS][100];

    printf("\nEnter number of stations (max %d): ", MAX_STATIONS);
    scanf("%d", &numStations);

    if (numStations <= 0 || numStations > MAX_STATIONS)
    {
        printf("Invalid number of stations.\n");
        return 1;
    }

    for (int i = 0; i < numStations; i++)
    {
        printf("Enter frame for Station %d: ", i + 1);
        scanf(" %99[^\n]", frames[i]);
    }

    for (int i = 0; i < numStations; i++)
    {
        transmitFrame(i + 1, frames[i]);
    }

    return 0;
}