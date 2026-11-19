#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/time.h>

#define PORT 8080
#define CHUNK_SIZE 5

// Custom structure for the UDP packet containing sequence metadata and 5-byte chunk
typedef struct {
    int seq_num;
    int total_chunks;
    char data[CHUNK_SIZE + 1]; // +1 for null termination when printing
} UDP_Packet;

// ==========================================
// RECEIVER LOGIC
// ==========================================
void run_receiver() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    srand(time(NULL) ^ (getpid() << 16));
    
    char reconstructed_message[2048] = {0};
    int received[500] = {0}; // Boolean array tracking received packets
    int expected_seq = 0;
    int total_chunks = -1;

    printf("\n[Receiver] Listening on UDP port %d...\n", PORT);

    while (1) {
        UDP_Packet pkt;
        if (recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client_addr, &client_len) > 0) {
            
            // Termination signal
            if (pkt.seq_num == -1) break; 
            if (total_chunks == -1) total_chunks = pkt.total_chunks;

            // Randomly drop with 20% probability to simulate real-world unreliability
            if (rand() % 100 < 20) {
                printf("[Receiver] X DROPPED packet %d (20%% loss simulation)\n", pkt.seq_num);
                continue; 
            }

            // Store packet in correct order if not already received
            if (!received[pkt.seq_num]) {
                received[pkt.seq_num] = 1;
                memcpy(reconstructed_message + (pkt.seq_num * CHUNK_SIZE), pkt.data, CHUNK_SIZE);
            }

            // Determine highest contiguous sequence number received
            while (received[expected_seq]) {
                expected_seq++;
            }

            // Send cumulative ACK (requesting the next expected sequence)
            sendto(sockfd, &expected_seq, sizeof(expected_seq), 0, (const struct sockaddr *)&client_addr, client_len);
            
            if (expected_seq == total_chunks) {
                printf("\n[Receiver] All chunks arrive. Reconstructed message:\n\"%s\"\n", reconstructed_message);
                break;
            }
        }
    }
    close(sockfd);
}

// ==========================================
// SENDER LOGIC
// ==========================================
void run_sender(const char *msg) {
    int sockfd;
    struct sockaddr_in server_addr;
    sleep(1); // Allow receiver to bind

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // Set 200ms timeout for recvfrom() to detect timeouts
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 200000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int total_len = strlen(msg);
    int total_chunks = (total_len + CHUNK_SIZE - 1) / CHUNK_SIZE;

    int cwnd = 1;          // Congestion Window 
    int ssthresh = 16;     // Slow Start Threshold
    int base = 0;          // Oldest unacknowledged sequence
    int next_seq = 0;      // Next sequence to send
    int dup_acks = 0;      // Duplicate ACK counter

    printf("[Sender] Total Chunks to send: %d (5 bytes each)\n", total_chunks);

    while (base < total_chunks) {
        // 1. Send chunks within current congestion window
        while (next_seq < base + cwnd && next_seq < total_chunks) {
            UDP_Packet pkt;
            pkt.seq_num = next_seq;
            pkt.total_chunks = total_chunks;
            memset(pkt.data, 0, sizeof(pkt.data));
            
            // Extract fixed-size 5-byte chunk
            int copy_len = (total_len - (next_seq * CHUNK_SIZE) >= CHUNK_SIZE) ? CHUNK_SIZE : (total_len - (next_seq * CHUNK_SIZE));
            strncpy(pkt.data, msg + (next_seq * CHUNK_SIZE), copy_len);

            sendto(sockfd, &pkt, sizeof(pkt), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
            printf("[Sender] Sent chunk %d: \"%s\"\n", pkt.seq_num, pkt.data);
            next_seq++;
        }

        // 2. Wait for ACK
        int ack;
        int recv_len = recvfrom(sockfd, &ack, sizeof(ack), 0, NULL, NULL);

        // 3. Congestion Control Logic
        if (recv_len < 0) {
            // TIMEOUT DETECTION
            printf("[Sender] ! TIMEOUT ! No ACKs received. (Congestion Detection)\n");
            ssthresh = (cwnd / 2 > 2) ? cwnd / 2 : 2;
            cwnd = 1; // Reset to 1 on timeout
            next_seq = base; // Retransmit from base
            dup_acks = 0;
            printf("          -> Retransmitting. New cwnd = %d, ssthresh = %d\n", cwnd, ssthresh);
        } else {
            if (ack == base) {
                // DUPLICATE ACK
                dup_acks++;
                printf("[Sender] Received duplicate ACK for %d (Count: %d)\n", ack, dup_acks);
                
                // FAST RETRANSMIT
                if (dup_acks == 3) {
                    printf("[Sender] !!! FAST RETRANSMIT TRIGGERED !!! (3 Duplicate ACKs)\n");
                    ssthresh = (cwnd / 2 > 2) ? cwnd / 2 : 2;
                    cwnd = ssthresh; // Adjust cwnd
                    next_seq = base; // Retransmit the missing packet immediately
                    dup_acks = 0;
                    printf("          -> Retransmitting %d. New cwnd = %d, ssthresh = %d\n", base, cwnd, ssthresh);
                }
            } else if (ack > base) {
                // NEW ACK RECEIVED
                printf("[Sender] Received valid ACK %d\n", ack);
                base = ack;
                dup_acks = 0; // Reset duplicate counter on successful advance
                
                if (cwnd < ssthresh) {
                    // SLOW START
                    cwnd *= 2; 
                    printf("[Sender] SLOW START: cwnd doubled to %d\n", cwnd);
                } else {
                    // CONGESTION AVOIDANCE
                    cwnd += 1;
                    printf("[Sender] CONGESTION AVOIDANCE: cwnd increased linearly to %d\n", cwnd);
                }
            }
        }
    }

    // Terminate connection cleanly
    UDP_Packet term_pkt;
    term_pkt.seq_num = -1;
    sendto(sockfd, &term_pkt, sizeof(term_pkt), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    close(sockfd);
}

int main() {
    char message[1024];

    printf("Enter the message to transmit: ");
    if (fgets(message, sizeof(message), stdin) != NULL) {
        size_t len = strlen(message);
        if (len > 0 && message[len-1] == '\n') {
            message[len-1] = '\0';
        }
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        run_sender(message);
        exit(EXIT_SUCCESS);
    } else {
        run_receiver();
        wait(NULL); 
    }

    return 0;
}