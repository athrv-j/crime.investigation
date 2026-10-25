#include <stdlib.h>
#include "../headers/dfs.h"

void dfsUtil(Graph *g, int node, int *visited, int *reachable, int *count) {
    visited[node] = 1;
    reachable[(*count)++] = node;
    
    EdgeNode *neighbor = getNeighbors(g, node);
    while (neighbor != NULL) {
        if (!visited[neighbor->destinationId]) {
            dfsUtil(g, neighbor->destinationId, visited, reachable, count);
        }
        neighbor = neighbor->next;
    }
}

DFSResult* dfs(Graph *g, int source) {
    DFSResult *result = (DFSResult*)malloc(sizeof(DFSResult));
    result->reachable = (int*)malloc(g->maxNodes * sizeof(int));
    result->count = 0;

    /* Guard: invalid source returns an empty result */
    if (source < 0 || source >= g->maxNodes)
        return result;
    
    int *visited = (int*)malloc(g->maxNodes * sizeof(int));
    for (int i = 0; i < g->maxNodes; i++) {
        visited[i] = 0;
    }
    
    dfsUtil(g, source, visited, result->reachable, &result->count);
    
    free(visited);
    return result;
}

void freeDFSResult(DFSResult *result) {
    free(result->reachable);
    free(result);
}