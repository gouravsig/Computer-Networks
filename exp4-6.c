#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 10
#define INF 9999

// Structure to represent a network link (edge)
struct Link {
    int source;
    int dest;
    int cost;
};

// Function to run Bellman-Ford and generate the routing table for a specific router
void calculateRoutingTable(int nodes, int linkCount, struct Link links[], int sourceNode) {
    int dist[MAX_NODES];
    int nextHop[MAX_NODES];

    // Initialize distances and next hops
    for (int i = 0; i < nodes; i++) {
        dist[i] = INF;
        nextHop[i] = -1;
    }
    
    dist[sourceNode] = 0;
    nextHop[sourceNode] = sourceNode;

    // Relax all links (nodes - 1) times
    for (int i = 1; i <= nodes - 1; i++) {
        for (int j = 0; j < linkCount; j++) {
            int u = links[j].source;
            int v = links[j].dest;
            int weight = links[j].cost;

            if (dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                
                // Determine the next hop
                if (u == sourceNode) {
                    nextHop[v] = v; // Direct connection
                } else {
                    nextHop[v] = nextHop[u]; // Route through existing path
                }
            }
        }
    }

    // Display the routing table
    printf("\nRouting Table for Router %d:\n", sourceNode);
    printf("Destination\tCost\tNext Hop\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < nodes; i++) {
        if (i == sourceNode) {
            printf("%d\t\t%d\t-\n", i, dist[i]);
        } else if (dist[i] == INF) {
            printf("%d\t\tINF\t-\n", i);
        } else {
            printf("%d\t\t%d\t%d\n", i, dist[i], nextHop[i]);
        }
    }
}

int main() {
    // Define a network with 4 routers (0, 1, 2, 3)
    int nodes = 4;
    
    // Bidirectional links between routers
    struct Link links[] = {
        {0, 1, 5}, {1, 0, 5},
        {0, 2, 4}, {2, 0, 4},
        {1, 2, 2}, {2, 1, 2},
        {1, 3, 3}, {3, 1, 3},
        {2, 3, 4}, {3, 2, 4}
    };
    
    int linkCount = sizeof(links) / sizeof(links[0]);

    printf("Distance Vector Routing Simulation\n");
    printf("Number of Routers: %d\n", nodes);

    // Generate routing table for each router in the network
    for (int i = 0; i < nodes; i++) {
        calculateRoutingTable(nodes, linkCount, links, i);
    }

    return 0;
}