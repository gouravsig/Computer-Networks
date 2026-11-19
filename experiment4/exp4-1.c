#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to check if a single octet is valid (0-255 and numeric)
int isValidOctet(char *str) {
    if (str == NULL || *str == '\0') return 0;
    int len = strlen(str);
    
    // An octet cannot be longer than 3 characters (e.g., "255")
    if (len > 3) return 0;
    
    // Check that every character is a digit
    for (int i = 0; i < len; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    
    // Convert to integer and check range
    int num = atoi(str);
    if (num >= 0 && num <= 255) {
        return 1;
    }
    return 0;
}

// Function to determine network class based on the first octet
void determineClass(int firstOctet) {
    if (firstOctet >= 0 && firstOctet <= 127) {
        printf("Class: A\n");
    } else if (firstOctet >= 128 && firstOctet <= 191) {
        printf("Class: B\n");
    } else if (firstOctet >= 192 && firstOctet <= 223) {
        printf("Class: C\n");
    } else if (firstOctet >= 224 && firstOctet <= 239) {
        printf("Class: D\n");
    } else if (firstOctet >= 240 && firstOctet <= 255) {
        printf("Class: E\n");
    }
}

void validateAndClassifyIP(char *ip) {
    char ipCopy[100];
    strncpy(ipCopy, ip, sizeof(ipCopy) - 1);
    ipCopy[sizeof(ipCopy) - 1] = '\0'; // Ensure null termination

    // Check for exactly 3 dots to prevent partial IPs from passing
    int dotCount = 0;
    for (int i = 0; ipCopy[i] != '\0'; i++) {
        if (ipCopy[i] == '.') dotCount++;
    }

    if (dotCount != 3) {
        printf("Result: Invalid IPv4 address\n");
        return;
    }

    int octets[4];
    int count = 0;
    
    // Split the string by dots
    char *token = strtok(ipCopy, ".");

    while (token != NULL) {
        if (!isValidOctet(token)) {
            printf("Result: Invalid IPv4 address\n");
            return;
        }
        if (count < 4) {
            octets[count] = atoi(token);
        }
        count++;
        token = strtok(NULL, ".");
    }

    // Ensure there were exactly 4 valid octets
    if (count != 4) {
        printf("Result: Invalid IPv4 address\n");
        return;
    }

    printf("Result: Valid IPv4 address\n");
    determineClass(octets[0]);
}

int main() {
    char ip[100];
    
    printf("Enter an IPv4 address: ");
    if (scanf("%99s", ip) == 1) {
        printf("Testing IP: %s\n", ip);
        validateAndClassifyIP(ip);
    }

    return 0;
}