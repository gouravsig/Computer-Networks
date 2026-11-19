
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#define MAX_SUBNETS 100

typedef struct
{
    int hosts_required;
    uint64_t subnet_size;
    int prefix;
    uint32_t subnet_address;
} Subnet;


int calculate_prefix(int hosts_required)
{
    int bits = 0;

    uint64_t required_addresses = (uint64_t)hosts_required + 2;

    while ((1ULL << bits) < required_addresses)
    {
        bits++;
    }

    if (bits > 30)
        return -1;

    return 32 - bits;
}

int compare(const void *a, const void *b)
{
    const Subnet *sa = (const Subnet *)a;
    const Subnet *sb = (const Subnet *)b;

    if (sa->hosts_required < sb->hosts_required)
        return 1;
    if (sa->hosts_required > sb->hosts_required)
        return -1;

    return 0;
}

void int_to_ip(uint32_t ip, char *buf)
{
    struct in_addr addr;

    addr.s_addr = htonl(ip);
    inet_ntop(AF_INET, &addr, buf, INET_ADDRSTRLEN);
}

int main()
{
    char major_net_str[30];
    char ip_part[20];

    int major_prefix;
    uint32_t major_ip, major_netmask;

    Subnet subnets[MAX_SUBNETS];
    int subnet_count;

    printf("Enter major network (e.g., 192.168.0.0/24): ");

    if (scanf("%29s", major_net_str) != 1)
        return 1;

    char *slash = strchr(major_net_str, '/');

    if (slash == NULL || strchr(slash + 1, '/') != NULL)
    {
        printf("Invalid format.\n");
        return 1;
    }

    *slash = '\0';

    if (strlen(major_net_str) >= sizeof(ip_part))
    {
        printf("Invalid IP address.\n");
        return 1;
    }

    strcpy(ip_part, major_net_str);

    char *endptr;
    long prefix_value = strtol(slash + 1, &endptr, 10);

    if (slash[1] == '\0' ||
        *endptr != '\0' ||
        prefix_value < 0 ||
        prefix_value > 32)
    {
        printf("Invalid prefix.\n");
        return 1;
    }

    major_prefix = (int)prefix_value;

    if (inet_pton(AF_INET, ip_part, &major_ip) != 1)
    {
        printf("Invalid IP address.\n");
        return 1;
    }

    major_ip = ntohl(major_ip);

    major_netmask = (major_prefix == 0)
                        ? 0
                        : (UINT32_MAX << (32 - major_prefix));

    major_ip = major_ip & major_netmask;

    uint64_t major_size = 1ULL << (32 - major_prefix);
    uint64_t end_of_major = (uint64_t)major_ip + major_size - 1;

    printf("Enter number of subnets to allocate: ");

    if (scanf("%d", &subnet_count) != 1 ||
        subnet_count < 1 || subnet_count > MAX_SUBNETS)
    {
        printf("Invalid number of subnets.\n");
        return 1;
    }

    for (int i = 0; i < subnet_count; i++)
    {
        printf("Enter number of hosts for subnet %d: ", i + 1);

        if (scanf("%d", &subnets[i].hosts_required) != 1 ||
            subnets[i].hosts_required < 1)
        {
            printf("Invalid host requirement.\n");
            return 1;
        }

        subnets[i].prefix =
            calculate_prefix(subnets[i].hosts_required);

        if (subnets[i].prefix == -1 ||
            subnets[i].prefix < major_prefix)
        {
            printf("Invalid or oversized subnet requirement.\n");
            return 1;
        }

        subnets[i].subnet_size =
            1ULL << (32 - subnets[i].prefix);
    }

    qsort(subnets, subnet_count, sizeof(Subnet), compare);

    uint64_t current_ip = major_ip;

    printf("\n--- VLSM Subnet Allocation ---\n");

    for (int i = 0; i < subnet_count; i++)
    {
        uint64_t size = subnets[i].subnet_size;

        current_ip = ((current_ip + size - 1) / size) * size;

        if (current_ip + size - 1 > end_of_major)
        {
            printf("Not enough space for subnet %d.\n", i + 1);
            return 1;
        }

        subnets[i].subnet_address = (uint32_t)current_ip;

        char ip_buf[INET_ADDRSTRLEN];

        int_to_ip(subnets[i].subnet_address, ip_buf);

        printf("Subnet %d: %s/%d (Usable Hosts: %llu)\n",
               i + 1,
               ip_buf,
               subnets[i].prefix,
               (unsigned long long)(size - 2));

        current_ip += size;
    }

    printf("\nAllocation completed successfully.\n");

    return 0;
}
