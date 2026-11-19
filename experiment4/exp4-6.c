
#include <stdio.h>
#include <stdlib.h>

#define MAX_NODES 10
#define INF 9999

struct Link {
    int source;
    int dest;
    int cost;
};

void calculateRoutingTable(int nodes, int linkCount,
                           struct Link links[], int sourceNode) {

    int dist[MAX_NODES];
    int nextHop[MAX_NODES];

    for (int i = 0; i < nodes; i++) {
        dist[i] = INF;
        nextHop[i] = -1;
    }

    dist[sourceNode] = 0;
    nextHop[sourceNode] = sourceNode;

    for (int i = 1; i <= nodes - 1; i++) {
        for (int j = 0; j < linkCount; j++) {

            int u = links[j].source;
            int v = links[j].dest;
            int weight = links[j].cost;

            if (dist[u] != INF &&
                dist[u] + weight < dist[v]) {

                dist[v] = dist[u] + weight;

                if (u == sourceNode)
                    nextHop[v] = v;
                else
                    nextHop[v] = nextHop[u];
            }
        }
    }

    printf("\nRouting Table for Router %d:\n", sourceNode);
    printf("Destination\tCost\tNext Hop\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < nodes; i++) {

        if (i == sourceNode) {
            printf("%d\t\t%d\t-\n", i, dist[i]);
        }
        else if (dist[i] == INF) {
            printf("%d\t\tINF\t-\n", i);
        }
        else {
            printf("%d\t\t%d\t%d\n",
                   i, dist[i], nextHop[i]);
        }
    }
}

int main() {

    int nodes, linkCount;

    struct Link links[MAX_NODES * MAX_NODES];

    printf("Distance Vector Routing Simulation\n");

    printf("Enter number of routers (max %d): ", MAX_NODES);
    scanf("%d", &nodes);

    if (nodes < 1 || nodes > MAX_NODES) {
        printf("Invalid number of routers!\n");
        return 1;
    }

    printf("Enter number of links: ");
    scanf("%d", &linkCount);

    if (linkCount < 0 || linkCount > MAX_NODES * MAX_NODES) {
        printf("Invalid number of links!\n");
        return 1;
    }

    printf("\nEnter links (source destination cost):\n");

    for (int i = 0; i < linkCount; i++) {

        printf("Link %d: ", i + 1);

        scanf("%d %d %d",
              &links[i].source,
              &links[i].dest,
              &links[i].cost);

        if (links[i].source < 0 || links[i].source >= nodes ||
            links[i].dest < 0 || links[i].dest >= nodes ||
            links[i].cost < 0) {

            printf("Invalid link information!\n");
            return 1;
        }
    }

    for (int i = 0; i < nodes; i++) {
        calculateRoutingTable(nodes, linkCount, links, i);
    }

    return 0;
}
