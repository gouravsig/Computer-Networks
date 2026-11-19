#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long long gcd(long long a, long long b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

long long modInverse(long long e, long long phi) {
    long long m0 = phi, t, q;
    long long x0 = 0, x1 = 1;

    if (phi == 1) return 0;

    while (e > 1) {
        q = e / phi;
        t = phi;
        phi = e % phi, e = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;
    return x1;
}

long long modExp(long long base, long long exp, long long mod) {
    long long res = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp % 2 == 1)
            res = (res * base) % mod;
        
        exp = exp >> 1; 
        base = (base * base) % mod;
    }
    return res;
}

int main() {
    long long p = 61;
    long long q = 53;

    long long n = p * q; 
    
    long long phi = (p - 1) * (q - 1); 

    long long e = 2;
    while (e < phi) {
        if (gcd(e, phi) == 1)
            break;
        e++;
    }

    long long d = modInverse(e, phi);

    printf("=== RSA Key Generation ===\n");
    printf("Chosen primes: p = %lld, q = %lld\n", p, q);
    printf("Public Key  (e, n): (%lld, %lld)\n", e, n);
    printf("Private Key (d, n): (%lld, %lld)\n\n", d, n);

    char message[1024];
    printf("Enter a text message to encrypt: ");
    if (fgets(message, sizeof(message), stdin) != NULL) {
        message[strcspn(message, "\n")] = 0; 
    }
    
    int len = strlen(message);
    long long encrypted[1024];
    char decrypted[1024];

    printf("\n=== Encryption ===\n");
    printf("Ciphertext (numeric array): ");
    for (int i = 0; i < len; i++) {
        encrypted[i] = modExp((long long)message[i], e, n);
        printf("%lld ", encrypted[i]);
    }
    printf("\n");

    printf("\n=== Decryption ===\n");
    for (int i = 0; i < len; i++) {
        decrypted[i] = (char)modExp(encrypted[i], d, n);
    }
    decrypted[len] = '\0';
    
    printf("Recovered Message: \"%s\"\n", decrypted);

    return 0;
}