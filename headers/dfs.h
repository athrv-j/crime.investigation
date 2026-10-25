#ifndef DFS_H
#define DFS_H

#include "graph.h"

typedef struct {
    int *reachable;
    int count;
} DFSResult;

DFSResult* dfs(Graph *g, int source);
void freeDFSResult(DFSResult *result);

#endif