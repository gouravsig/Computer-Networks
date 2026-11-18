#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define IP_MF 0x2000          // More Fragments flag (bit 13)
#define IP_OFFSET_MASK 0x1FFF // Mask for the 13-bit fragment offset

// Struct simulating fields from an IPv4 header
typedef struct {
    uint8_t version_ihl;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    char source_ip[16];       // Stored as strings as requested
    char dest_ip[16];
    char *data;               // Payload for the text message
} IPFragment;

// Displays detailed header information for each fragment
void printFragmentHeader(IPFragment *frag, int frag_num) {
    int mf = (frag->frag_off & IP_MF) >> 13;
    int offset = (frag->frag_off & IP_OFFSET_MASK) * 8;
    
    printf("\nFragment %d:\n", frag_num);
    printf("  Source IP: %s\n", frag->source_ip);
    printf("  Dest IP: %s\n", frag->dest_ip);
    printf("  ID: %u\n", frag->id);
    printf("  Total Length: %u bytes\n", frag->tot_len);
    printf("  Flags: MF=%d\n", mf);
    printf("  Offset: %d bytes\n", offset);
    printf("  Payload Data: \"%s\"\n", frag->data);
}

// Breaks the message into smaller fragments based on MTU
IPFragment* fragmentMessage(const char *message, int mtu, uint16_t id, int *num_frags) {
    int ip_hdr_len = 20;
    int original_data_len = strlen(message);
    
    // Check that MTU exceeds header size
    if (mtu <= ip_hdr_len) {
        printf("Error: MTU must be greater than header size (20 bytes).\n");
        exit(1);
    }

    int max_data_per_frag = mtu - ip_hdr_len;
    max_data_per_frag = (max_data_per_frag / 8) * 8; // Ensure 8-byte boundary alignment

    if (max_data_per_frag == 0) {
        printf("Error: MTU is too small to carry any payload aligned to 8 bytes.\n");
        exit(1);
    }

    *num_frags = (original_data_len + max_data_per_frag - 1) / max_data_per_frag;
    if (original_data_len == 0) *num_frags = 1; 

    IPFragment *fragments = malloc(*num_frags * sizeof(IPFragment));
    
    int remaining_data = original_data_len;
    int current_offset = 0;
    const char *msg_ptr = message;

    for (int i = 0; i < *num_frags; i++) {
        fragments[i].version_ihl = 0x45;
        fragments[i].ttl = 64;
        fragments[i].protocol = 17;
        fragments[i].id = id;
        strcpy(fragments[i].source_ip, "192.168.1.1");
        strcpy(fragments[i].dest_ip, "192.168.1.2");

        int data_len = (remaining_data > max_data_per_frag) ? max_data_per_frag : remaining_data;
        fragments[i].tot_len = ip_hdr_len + data_len;

        uint16_t offset_val = current_offset / 8;
        if (remaining_data > max_data_per_frag) {
            fragments[i].frag_off = IP_MF | offset_val; // Set MF flag
        } else {
            fragments[i].frag_off = offset_val;         // Clear MF flag
        }

        // Allocate memory for this fragment's text payload and copy it over
        fragments[i].data = calloc(1, data_len + 1); // +1 for null terminator to print safely
        strncpy(fragments[i].data, msg_ptr, data_len);

        remaining_data -= data_len;
        current_offset += data_len;
        msg_ptr += data_len;
    }

    return fragments;
}

// Concatenates all payloads from the fragments back into a single message
void reassembleMessage(IPFragment *fragments, int num_frags) {
    printf("\n--- REASSEMBLY LOGIC ---\n");
    int total_len = 0;
    
    for(int i = 0; i < num_frags; i++) {
        total_len += (fragments[i].tot_len - 20);
    }
    
    char *reassembled_msg = calloc(1, total_len + 1);
    
    for (int i = 0; i < num_frags; i++) {
        strcat(reassembled_msg, fragments[i].data);
        if ((fragments[i].frag_off & IP_MF) == 0) {
            printf(">> Last fragment received (MF=0). Reassembly complete.\n");
        }
    }
    
    printf("\nReassembled Message: \"%s\"\n", reassembled_msg);
    free(reassembled_msg);
}

int main() {
    char message[4096];
    int mtu;
    uint16_t packet_id = 9999;

    // 1. Take input for the message
    printf("Enter the text message to transmit: ");
    if (fgets(message, sizeof(message), stdin) != NULL) {
        size_t len = strlen(message);
        if (len > 0 && message[len-1] == '\n') {
            message[len-1] = '\0'; // Strip newline
        }
    }

    // 2. Take input for the MTU size
    printf("Enter the MTU size (e.g., 36, 1500): ");
    scanf("%d", &mtu);

    printf("\nOriginal Message Length: %zu bytes\n", strlen(message));
    printf("MTU: %d bytes\n", mtu);

    int num_frags = 0;
    
    // 3. Call fragmentation
    IPFragment *fragments = fragmentMessage(message, mtu, packet_id, &num_frags);

    printf("\n--- FRAGMENTATION LOGIC ---");
    // 4. Print each fragment
    for (int i = 0; i < num_frags; i++) {
        printFragmentHeader(&fragments[i], i + 1);
    }

    // 5. Reassemble and display
    reassembleMessage(fragments, num_frags);

    // Free allocated memory
    for (int i = 0; i < num_frags; i++) {
        free(fragments[i].data);
    }
    free(fragments);

    return 0;
}