#include <stdlib.h>
#include <string.h>
#include "../headers/bfs.h"

BFSResult* bfs(Graph *g, int source, int target) {
    BFSResult *result = (BFSResult*)malloc(sizeof(BFSResult));
    result->path = (int*)malloc(g->maxNodes * sizeof(int));
    result->pathLength = 0;

    /* Guard: invalid IDs return an empty result instead of corrupting memory */
    if (source < 0 || source >= g->maxNodes ||
        target < 0 || target >= g->maxNodes)
        return result;
    
    // Create queue
    int *queue = (int*)malloc(g->maxNodes * sizeof(int));
    int front = 0, rear = 0;
    
    // Create visited and parent arrays
    int *visited = (int*)malloc(g->maxNodes * sizeof(int));
    int *parent = (int*)malloc(g->maxNodes * sizeof(int));
    
    for (int i = 0; i < g->maxNodes; i++) {
        visited[i] = 0;
        parent[i] = -1;
    }
    
    // Start BFS
    queue[rear++] = source;
    visited[source] = 1;
    
    while (front < rear) {
        int current = queue[front++];
        
        if (current == target) {
            // Reconstruct path
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
            
            free(queue);
            free(visited);
            free(parent);
            return result;
        }
        
        // Explore neighbors
        EdgeNode *neighbor = getNeighbors(g, current);
        while (neighbor != NULL) {
            if (!visited[neighbor->destinationId]) {
                visited[neighbor->destinationId] = 1;
                parent[neighbor->destinationId] = current;
                queue[rear++] = neighbor->destinationId;
            }
            neighbor = neighbor->next;
        }
    }
    
    // No path found
    free(queue);
    free(visited);
    free(parent);
    return result;
}

void freeBFSResult(BFSResult *result) {
    free(result->path);
    free(result);
}