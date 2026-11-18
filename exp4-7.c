#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 20
#define INF 9999

// Function to find the unvisited node with the minimum distance
int minDistance(int dist[], bool visited[], int nodes) {
    int min = INF, min_index = -1;
    for (int v = 0; v < nodes; v++) {
        if (!visited[v] && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// Function to run Dijkstra's algorithm and print the routing table
void runDijkstra(int graph[MAX_NODES][MAX_NODES], int nodes, int src) {
    int dist[MAX_NODES];     // Shortest distance from src to i
    bool visited[MAX_NODES]; // True if node is included in shortest path tree
    int parent[MAX_NODES];   // Array to store shortest path tree (used for next hop)

    // Initialize arrays
    for (int i = 0; i < nodes; i++) {
        dist[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < nodes - 1; count++) {
        int u = minDistance(dist, visited, nodes);
        if (u == -1) break; // Remaining nodes are unreachable
        
        visited[u] = true;

        // Update dist value of adjacent vertices
        for (int v = 0; v < nodes; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    // Display the routing table for the current router
    printf("\nRouting Table for Router %d:\n", src);
    printf("Destination\tCost\tNext Hop\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < nodes; i++) {
        if (i == src) {
            printf("%d\t\t%d\t-\n", i, dist[i]);
        } else if (dist[i] == INF) {
            printf("%d\t\tINF\t-\n", i);
        } else {
            // Trace back the parent array to find the first hop from the source
            int nextHop = i;
            while (parent[nextHop] != src && parent[nextHop] != -1) {
                nextHop = parent[nextHop];
            }
            printf("%d\t\t%d\t%d\n", i, dist[i], nextHop);
        }
    }
}

int main() {
    int nodes, edges;
    int graph[MAX_NODES][MAX_NODES] = {0};

    // Dynamically take the network topology from the user
    printf("Enter the number of routers: ");
    scanf("%d", &nodes);

    printf("Enter the number of bidirectional links: ");
    scanf("%d", &edges);

    printf("\nEnter link details (Source Destination Cost):\n");
    for (int i = 0; i < edges; i++) {
        int u, v, cost;
        printf("Link %d: ", i + 1);
        scanf("%d %d %d", &u, &v, &cost);
        
        // Populate adjacency matrix (assuming bidirectional/undirected links)
        graph[u][v] = cost;
        graph[v][u] = cost;
    }

    printf("\n--- Link State Routing Simulation (Dijkstra) ---");
    
    // Generate routing table for each router in the network
    for (int i = 0; i < nodes; i++) {
        runDijkstra(graph, nodes, i);
    }

    return 0;
}