#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define DNS_SERVER "8.8.8.8"
#define DNS_PORT 53

// Standard DNS Header structure
struct DNS_HEADER {
    unsigned short id;       // Identification number
    unsigned short flags;    // DNS Flags
    unsigned short q_count;  // Number of questions
    unsigned short ans_count;// Number of answers
    unsigned short auth_count;// Number of authority records
    unsigned short add_count; // Number of additional records
};

// DNS Question structure
struct QUESTION {
    unsigned short qtype;
    unsigned short qclass;
};

// Utility function to format domain name (e.g., www.google.com -> 3www6google3com0)
void format_domain_name(unsigned char* dns, unsigned char* host) {
    int lock = 0, i;
    strcat((char*)host, ".");
    
    for(i = 0; i < strlen((char*)host); i++) {
        if(host[i] == '.') {
            *dns++ = i - lock;
            for(; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++;
        }
    }
    *dns++ = '\0';
}

int main() {
    int sockfd;
    struct sockaddr_in dest;
    unsigned char buf[65536], *qname, *reader;
    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
    
    char hostname[256];
    printf("Enter domain name to resolve (e.g., www.example.com): ");
    scanf("%s", hostname);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    dest.sin_family = AF_INET;
    dest.sin_port = htons(DNS_PORT);
    dest.sin_addr.s_addr = inet_addr(DNS_SERVER);

    // Construct DNS Header
    dns = (struct DNS_HEADER *)&buf;
    dns->id = (unsigned short) htons(getpid());
    dns->flags = htons(0x0100); // Standard query, Recursion Desired (RD = 1)
    dns->q_count = htons(1);
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    // Construct DNS Question Name
    qname = (unsigned char*)&buf[sizeof(struct DNS_HEADER)];
    format_domain_name(qname, (unsigned char*)hostname);

    // Construct DNS Question Info
    qinfo = (struct QUESTION*)&buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1)];
    qinfo->qtype = htons(1); // Type A (IPv4)
    qinfo->qclass = htons(1); // Class IN (Internet)

    int query_size = sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION);

    // Send the query
    printf("\nSending DNS Query to %s...\n", DNS_SERVER);
    if(sendto(sockfd, (char*)buf, query_size, 0, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
        perror("Send failed");
        return 1;
    }

    // Receive the response
    socklen_t addr_len = sizeof(dest);
    int response_size = recvfrom(sockfd, (char*)buf, 65536, 0, (struct sockaddr*)&dest, &addr_len);
    if(response_size < 0) {
        perror("Receive failed");
        return 1;
    }

    printf("Response received. Parsing answer...\n\n");

    dns = (struct DNS_HEADER*) buf;
    int ans_count = ntohs(dns->ans_count);
    
    if (ans_count == 0) {
        printf("No DNS records found.\n");
        close(sockfd);
        return 0;
    }

    // Move reader past the header and the original question to reach the answer section
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION)];

    // Parse the Answer section (Simplified for standard Type A records)
    for(int i = 0; i < ans_count; i++) {
        // Skip the name pointer (usually 2 bytes, starts with 11 for compression)
        reader += 2; 

        unsigned short type = ntohs(*(unsigned short*)reader);
        reader += 2; // skip type
        reader += 2; // skip class
        reader += 4; // skip TTL

        unsigned short data_len = ntohs(*(unsigned short*)reader);
        reader += 2; // skip data length

        if (type == 1) { // Type A record (IPv4)
            struct sockaddr_in a;
            long *p = (long*)reader;
            a.sin_addr.s_addr = (*p);
            printf("Resolved IPv4 Address: %s\n", inet_ntoa(a.sin_addr));
        }
        reader += data_len; // move to the next record
    }

    close(sockfd);
    return 0;
}

