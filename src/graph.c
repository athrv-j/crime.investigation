#include <stdlib.h>
#include <string.h>
#include "../headers/graph.h"

Graph* createGraph(int maxNodes)
{
    if (maxNodes <= 0)
        return NULL;

    Graph *g = malloc(sizeof(Graph));
    if (g == NULL)
        return NULL;

    g->adjacencyList = calloc((size_t)maxNodes, sizeof(EdgeNode*));
    g->inDegree      = calloc((size_t)maxNodes, sizeof(int));
    g->nodeExists    = calloc((size_t)maxNodes, sizeof(int));

    if (g->adjacencyList == NULL || g->inDegree == NULL || g->nodeExists == NULL)
    {
        free(g->adjacencyList);
        free(g->inDegree);
        free(g->nodeExists);
        free(g);
        return NULL;
    }

    g->maxNodes  = maxNodes;
    g->nodeCount = 0;
    return g;
}

void addNode(Graph *g, int nodeId)
{
    if (g == NULL || nodeId < 0 || nodeId >= g->maxNodes)
        return;

    if (g->nodeExists[nodeId] == 0)
    {
        g->nodeExists[nodeId] = 1;
        g->nodeCount++;
    }
}

int addEdge(Graph *g, int sourceId, int destId, char *type, int weight)
{
    if (g == NULL || type == NULL ||
        sourceId < 0 || sourceId >= g->maxNodes ||
        destId < 0 || destId >= g->maxNodes)
    {
        return 0;
    }

    EdgeNode *newEdge = malloc(sizeof(EdgeNode));
    if (newEdge == NULL)
        return 0;

    newEdge->destinationId = destId;
    strncpy(newEdge->type, type, sizeof(newEdge->type) - 1);
    newEdge->type[sizeof(newEdge->type) - 1] = '\0';
    newEdge->weight = weight;
    newEdge->timespan = 0;
    newEdge->next = NULL;

    /* Append to the end of the source node's list */
    if (g->adjacencyList[sourceId] == NULL)
    {
        g->adjacencyList[sourceId] = newEdge;
    }
    else
    {
        EdgeNode *current = g->adjacencyList[sourceId];
        while (current->next != NULL)
            current = current->next;
        current->next = newEdge;
    }

    addNode(g, sourceId);
    addNode(g, destId);
    g->inDegree[destId]++;
    return 1;
}

EdgeNode* getNeighbors(Graph *g, int nodeId)
{
    if (g == NULL || nodeId < 0 || nodeId >= g->maxNodes)
        return NULL;
    return g->adjacencyList[nodeId];
}

int getInDegree(Graph *g, int nodeId)
{
    if (g == NULL || nodeId < 0 || nodeId >= g->maxNodes)
        return 0;
    return g->inDegree[nodeId];
}

void freeGraph(Graph *g)
{
    if (g == NULL)
        return;

    for (int i = 0; i < g->maxNodes; i++)
    {
        EdgeNode *current = g->adjacencyList[i];
        while (current != NULL)
        {
            EdgeNode *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(g->adjacencyList);
    free(g->inDegree);
    free(g->nodeExists);   /* freed once, after the loop */
    free(g);
}
