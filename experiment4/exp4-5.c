#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HEADER_SIZE 20
#define MAX_FRAGMENTS 50
#define MAX_DATA_LEN 1000
#define IP_VERSION 4
#define IP_IHL 5
#define TTL 64
#define PROTOCOL_TCP 6

typedef struct
{
    int version;
    int ihl;
    int total_length;
    int identification;
    int flags;
    int frag_offset;
    int ttl;
    int protocol;
    int checksum;
    char src_ip[16];
    char dest_ip[16];
    char data[MAX_DATA_LEN + 1];
} IPFragment;


// Fragment the original message
int fragmentMessage(const char *message, int mtu,
                    IPFragment fragments[], int identification)
{
    int message_len = strlen(message);
    int available = mtu - HEADER_SIZE;

    if (available <= 0)
        return -1;

    if (message_len == 0)
        return 0;

    // Maximum payload must be a multiple of 8
    int max_payload = (available / 8) * 8;

    // If the message fits in one packet, alignment is unnecessary
    if (message_len > available && max_payload == 0)
        return -1;

    int offset = 0;
    int index = 0;
    int num_fragments = 0;

    while (index < message_len)
    {
        if (num_fragments >= MAX_FRAGMENTS)
            return -1;

        IPFragment *frag = &fragments[num_fragments];

        frag->version = IP_VERSION;
        frag->ihl = IP_IHL;
        frag->identification = identification;
        frag->frag_offset = offset / 8;
        frag->ttl = TTL;
        frag->protocol = PROTOCOL_TCP;
        frag->checksum = 0;

        strcpy(frag->src_ip, "192.168.0.1");
        strcpy(frag->dest_ip, "192.168.0.2");

        int remaining = message_len - index;

        int copy_len = (remaining > available)
                       ? max_payload : remaining;

        // Copy message data into fragment
        memcpy(frag->data, message + index, copy_len);
        frag->data[copy_len] = '\0';

        // Set MF flag
        frag->flags = (index + copy_len < message_len) ? 1 : 0;

        // Calculate total fragment length
        frag->total_length = HEADER_SIZE + copy_len;

        index += copy_len;
        offset += copy_len;
        num_fragments++;
    }

    return num_fragments;
}


// Display fragment header information
void printFragmentHeader(IPFragment *frag, int index)
{
    printf("\nFragment %d:\n", index + 1);

    printf("  Version: %d\n", frag->version);
    printf("  IHL: %d (Header size: %d bytes)\n",
           frag->ihl, frag->ihl * 4);

    printf("  Total Length: %d bytes\n", frag->total_length);
    printf("  ID: %d\n", frag->identification);
    printf("  Flags (MF): %d\n", frag->flags);

    printf("  Fragment Offset: %d (bytes: %d)\n",
           frag->frag_offset, frag->frag_offset * 8);

    printf("  TTL: %d\n", frag->ttl);
    printf("  Protocol: %d\n", frag->protocol);

    printf("  Source IP: %s\n", frag->src_ip);
    printf("  Destination IP: %s\n", frag->dest_ip);

    printf("  Data: \"%s\"\n", frag->data);
}


// Reassemble fragments using their offsets
int reassembleMessage(IPFragment fragments[], int num_fragments,
                      char output[], int output_size)
{
    memset(output, 0, output_size);

    for (int i = 0; i < num_fragments; i++)
    {
        int offset = fragments[i].frag_offset * 8;
        int data_len = fragments[i].total_length - HEADER_SIZE;

        if (offset < 0 || data_len < 0 ||
            offset + data_len >= output_size)
            return 0;

        memcpy(output + offset, fragments[i].data, data_len);
    }

    return 1;
}


// Main function
int main()
{
    char message[MAX_DATA_LEN];
    char reassembled[MAX_DATA_LEN];

    int mtu;
    int id = 12345;

    printf("Enter the message to be fragmented: ");

    if (fgets(message, sizeof(message), stdin) == NULL)
        return 1;

    message[strcspn(message, "\n")] = '\0';

    printf("Enter the MTU size: ");

    if (scanf("%d", &mtu) != 1 || mtu <= HEADER_SIZE)
    {
        printf("Invalid MTU size.\n");
        return 1;
    }

    IPFragment fragments[MAX_FRAGMENTS];

    int count = fragmentMessage(message, mtu, fragments, id);

    if (count < 0)
    {
        printf("Fragmentation failed. MTU too small or too many fragments.\n");
        return 1;
    }

    printf("\n--- IP FRAGMENTS ---\n");

    for (int i = 0; i < count; i++)
    {
        printFragmentHeader(&fragments[i], i);
    }

    if (reassembleMessage(fragments, count,
                          reassembled, sizeof(reassembled)))
    {
        printf("\n--- REASSEMBLED MESSAGE ---\n");
        printf("%s\n", reassembled);
    }
    else
    {
        printf("Reassembly failed.\n");
    }

    return 0;
}