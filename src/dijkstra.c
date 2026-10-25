#include <stdlib.h>
#include <limits.h>
#include "../headers/dijkstra.h"

DijkstraResult* dijkstra(Graph *g, int source, int target) {
    DijkstraResult *result = (DijkstraResult*)malloc(sizeof(DijkstraResult));
    result->path = (int*)malloc(g->maxNodes * sizeof(int));
    result->pathLength = 0;
    result->distance = INT_MAX;

    /* Guard: invalid IDs return an empty result instead of corrupting memory */
    if (source < 0 || source >= g->maxNodes ||
        target < 0 || target >= g->maxNodes)
        return result;
    
    // Initialize distance and parent arrays
    int *distance = (int*)malloc(g->maxNodes * sizeof(int));
    int *parent = (int*)malloc(g->maxNodes * sizeof(int));
    int *visited = (int*)malloc(g->maxNodes * sizeof(int));
    
    for (int i = 0; i < g->maxNodes; i++) {
        distance[i] = INT_MAX;
        parent[i] = -1;
        visited[i] = 0;
    }
    
    distance[source] = 0;
    
    // Dijkstra's algorithm
    for (int count = 0; count < g->maxNodes; count++) {
        int u = -1;
        int minDist = INT_MAX;
        
        // Find unvisited node with minimum distance
        for (int i = 0; i < g->maxNodes; i++) {
            if (!visited[i] && distance[i] < minDist) {
                minDist = distance[i];
                u = i;
            }
        }
        
        if (u == -1) break;
        
        visited[u] = 1;
        
        if (u == target) break;
        
        // Update distances
        EdgeNode *neighbor = getNeighbors(g, u);
        while (neighbor != NULL) {
            int v = neighbor->destinationId;
            int weight = neighbor->weight;
            
            if (!visited[v] && distance[u] != INT_MAX && 
                distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                parent[v] = u;
            }
            neighbor = neighbor->next;
        }
    }
    
    // Reconstruct path
    if (distance[target] != INT_MAX) {
        int node = target;
        int pathIdx = 0;
        while (node != -1) {
            result->path[pathIdx++] = node;
            node = parent[node];
        }
        result->pathLength = pathIdx;
        
        // Reverse path
        for (int i = 0; i < pathIdx / 2; i++) {
            int temp = result->path[i];
            result->path[i] = result->path[pathIdx - 1 - i];
            result->path[pathIdx - 1 - i] = temp;
        }
        
        result->distance = distance[target];
    }
    
    free(distance);
    free(parent);
    free(visited);
    return result;
}

void freeDijkstraResult(DijkstraResult *result) {
    free(result->path);
    free(result);
}