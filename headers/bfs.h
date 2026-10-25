#ifndef BFS_H
#define BFS_H

#include "graph.h"

typedef struct {
    int *path;
    int pathLength;
} BFSResult;

BFSResult* bfs(Graph *g, int source, int target);
void freeBFSResult(BFSResult *result);

#endif