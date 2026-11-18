#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 8080
#define CHUNK_SIZE 16

// UDP Packet Structure mimicking a real header
typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
    char data[CHUNK_SIZE];
} UDP_Packet;

// Simulates random packet loss with a 25% probability
int simulate_loss() {
    return (rand() % 100) < 25; // Returns 1 (true) if packet should be dropped
}

// Receiver Logic
void run_receiver(int expected_packets) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    UDP_Packet recv_pkt;
    
    char reconstructed_message[1024] = ""; // Buffer for final message

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("[Receiver] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[Receiver] Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("\n[Receiver] Listening on port %d...\n", PORT);
    srand(time(NULL) ^ (getpid() << 16)); // Seed random number generator

    for (int i = 0; i < expected_packets; i++) {
        recvfrom(sockfd, &recv_pkt, sizeof(UDP_Packet), 0, (struct sockaddr *)&client_addr, &client_len);
        
        printf("[Receiver] Received packet %d... ", i + 1);
        
        // Simulate packet loss at the application layer
        if (simulate_loss()) {
            printf("DROPPED (Simulated Loss)\n");
        } else {
            printf("ACCEPTED\n");
            printf("  -> Header: SrcPort=%u, DestPort=%u, Len=%u, Checksum=%u\n", 
                   recv_pkt.src_port, recv_pkt.dest_port, recv_pkt.length, recv_pkt.checksum);
            printf("  -> Data: \"%.*s\"\n", CHUNK_SIZE, recv_pkt.data);
            
            // Append to reconstructed message
            strncat(reconstructed_message, recv_pkt.data, CHUNK_SIZE);
        }
    }

    printf("\n[Receiver] Final Reconstructed Message: \"%s\"\n", reconstructed_message);
    close(sockfd);
}

// Sender Logic
void run_sender(const char *message, int num_packets) {
    int sockfd;
    struct sockaddr_in server_addr;
    UDP_Packet send_pkt;

    sleep(1); // Give receiver time to bind

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("[Sender] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("\n[Sender] Starting transmission of %d packets...\n", num_packets);

    for (int i = 0; i < num_packets; i++) {
        // Fill header fields
        send_pkt.src_port = 54321;
        send_pkt.dest_port = PORT;
        send_pkt.length = sizeof(UDP_Packet);
        send_pkt.checksum = 0; // Placeholder
        
        // Fill 16-byte data payload
        memset(send_pkt.data, 0, CHUNK_SIZE);
        strncpy(send_pkt.data, message + (i * CHUNK_SIZE), CHUNK_SIZE);

        printf("[Sender] Sending packet %d\n", i + 1);
        printf("  -> Header: SrcPort=%u, DestPort=%u, Len=%u, Checksum=%u\n", 
               send_pkt.src_port, send_pkt.dest_port, send_pkt.length, send_pkt.checksum);
        printf("  -> Data: \"%.*s\"\n", CHUNK_SIZE, send_pkt.data);

        sendto(sockfd, &send_pkt, sizeof(UDP_Packet), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        usleep(100000); // Small delay to prevent network buffer overflow
    }

    close(sockfd);
}

int main() {
    char message[1024];

    // User inputs the message
    printf("Enter the message to transmit: ");
    if (fgets(message, sizeof(message), stdin) != NULL) {
        size_t len = strlen(message);
        if (len > 0 && message[len-1] == '\n') {
            message[len-1] = '\0';
        }
    }

    int msg_len = strlen(message);
    int num_packets = (msg_len + CHUNK_SIZE - 1) / CHUNK_SIZE;
    if (num_packets == 0) num_packets = 1;

    // Use fork() to simulate both sender and receiver in the same program
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process acts as the Sender
        run_sender(message, num_packets);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process acts as the Receiver
        run_receiver(num_packets);
        wait(NULL); 
    }

    return 0;
}