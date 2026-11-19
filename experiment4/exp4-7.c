#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 20
#define INF 9999

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

void runDijkstra(int graph[MAX_NODES][MAX_NODES], int nodes, int src) {
    int dist[MAX_NODES];    
    bool visited[MAX_NODES];
    int parent[MAX_NODES];   

    for (int i = 0; i < nodes; i++) {
        dist[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[src] = 0;

    for (int count = 0; count < nodes - 1; count++) {
        int u = minDistance(dist, visited, nodes);
        if (u == -1) break; 
        
        visited[u] = true;

        for (int v = 0; v < nodes; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    printf("\nRouting Table for Router %d:\n", src);
    printf("Destination\tCost\tNext Hop\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < nodes; i++) {
        if (i == src) {
            printf("%d\t\t%d\t-\n", i, dist[i]);
        } else if (dist[i] == INF) {
            printf("%d\t\tINF\t-\n", i);
        } else {
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

    printf("Enter the number of routers: ");
    scanf("%d", &nodes);

    printf("Enter the number of bidirectional links: ");
    scanf("%d", &edges);

    printf("\nEnter link details (Source Destination Cost):\n");
    for (int i = 0; i < edges; i++) {
        int u, v, cost;
        printf("Link %d: ", i + 1);
        scanf("%d %d %d", &u, &v, &cost);
        
        graph[u][v] = cost;
        graph[v][u] = cost;
    }

    printf("\n--- Link State Routing Simulation (Dijkstra) ---");
    
    for (int i = 0; i < nodes; i++) {
        runDijkstra(graph, nodes, i);
    }

    return 0;
}