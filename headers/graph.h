#ifndef GRAPH_H
#define GRAPH_H

#include "struct.h"

/* One directed edge in a node's adjacency list. */
typedef struct EdgeNode {
    int destinationId;
    char type[100];
    int weight;
    time_t timespan;
    struct EdgeNode *next;
} EdgeNode;

/* Adjacency-list graph. Node IDs are array indices (0 .. maxNodes-1). */
typedef struct {
    EdgeNode **adjacencyList;
    int *inDegree;
    int *nodeExists;    /* 1 if the node has been added, else 0 */
    int maxNodes;
    int nodeCount;
} Graph;

Graph*    createGraph(int maxNodes);             /* NULL on failure */
void      addNode(Graph *g, int nodeId);         /* registers a node once */
int       addEdge(Graph *g, int sourceId, int destId,
                  char *type, int weight);       /* 1 = added, 0 = failed */
EdgeNode* getNeighbors(Graph *g, int nodeId);
int       getInDegree(Graph *g, int nodeId);
void      freeGraph(Graph *g);

#endif
