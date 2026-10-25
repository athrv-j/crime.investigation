#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

typedef struct {
    int* path ;
    int pathLength;
    int distance;
} DijkstraResult;

DijkstraResult* dijkstra(Graph* g, int source, int target);
void freeDijkstraResult(DijkstraResult* result);
#endif