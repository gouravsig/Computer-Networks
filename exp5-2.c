#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Connection states
#define CLOSED 0
#define ESTABLISHED 1

int main() {
    int choice;
    int state = CLOSED;
    unsigned int client_seq = 0;
    unsigned int server_seq = 0;
    
    srand(time(NULL));

    while (1) {
        printf("\n=========================================\n");
        printf("          TCP SIMULATION MENU\n");
        printf("=========================================\n");
        printf("1. Connection Establishment (3-Way Handshake)\n");
        printf("2. Data Transmission\n");
        printf("3. Connection Termination (4-Way Handshake)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // clear buffer
            continue;
        }
        getchar(); // consume newline

        switch (choice) {
            case 1:
                if (state == ESTABLISHED) {
                    printf("\n[!] Connection is already established.\n");
                    break;
                }
                printf("\n--- PHASE 1: Connection Establishment ---\n");
                
                // Random sequence numbers for realism
                client_seq = rand() % 10000 + 1000;
                server_seq = rand() % 10000 + 5000;
                
                printf("[Client] -> [Server] : SYN  (Seq=%u)\n", client_seq);
                printf("[Server] -> [Client] : SYN-ACK (Seq=%u, Ack=%u)\n", server_seq, client_seq + 1);
                
                client_seq++;
                server_seq++;
                
                printf("[Client] -> [Server] : ACK  (Seq=%u, Ack=%u)\n", client_seq, server_seq);
                printf("\n>>> Connection successfully established! <<<\n");
                
                state = ESTABLISHED;
                break;

            case 2:
                if (state == CLOSED) {
                    printf("\n[!] You must establish a connection first (Option 1).\n");
                    break;
                }
                
                printf("\n--- PHASE 2: Data Transmission ---\n");
                int num_packets;
                printf("Enter the number of data packets to send: ");
                scanf("%d", &num_packets);
                getchar(); // consume newline
                
                for (int i = 1; i <= num_packets; i++) {
                    char data[256];
                    printf("\nEnter custom data for Packet %d: ", i);
                    fgets(data, sizeof(data), stdin);
                    data[strcspn(data, "\n")] = 0; // Remove newline
                    
                    int data_len = strlen(data);
                    if (data_len == 0) data_len = 1; // Prevent zero-length seq increment
                    
                    // Client sends DATA
                    printf("[Client] -> [Server] : [DATA] Seq=%u, Payload=\"%s\"\n", client_seq, data);
                    
                    // Server replies with ACK
                    client_seq += data_len; // Sequence number increases by data length
                    printf("[Server] -> [Client] : ACK (Ack=%u)\n", client_seq);
                }
                break;

            case 3:
                if (state == CLOSED) {
                    printf("\n[!] No active connection to terminate.\n");
                    break;
                }
                printf("\n--- PHASE 3: Connection Termination ---\n");
                
                printf("[Client] -> [Server] : FIN\n");
                printf("[Server] -> [Client] : ACK\n");
                printf("[Server] -> [Client] : FIN\n");
                printf("[Client] -> [Server] : ACK\n");
                
                printf("\n>>> Connection closed cleanly. <<<\n");
                state = CLOSED;
                break;

            case 4:
                printf("\nExiting TCP Simulation. Goodbye!\n");
                exit(0);

            default:
                printf("\n[!] Invalid choice. Please select 1-4.\n");
        }
    }
    return 0;
}