
#include <stdlib.h>
#include "../headers/cycle.h"
#include "../headers/graph.h"

// DFS helper function
static int visitCycle(Graph *g, int node, int color[])
{
    color[node] = 1;  // Gray: currently exploring

    EdgeNode *neighbor = getNeighbors(g, node);

    while (neighbor != NULL)
    {
        int v = neighbor->destinationId;

        if (v < 0 || v >= g->maxNodes)
        {
            neighbor = neighbor->next;
            continue;
        }

        if (color[v] == 1)
        {
            // Back edge found: cycle detected
            return 1;
        }

        if (color[v] == 0)
        {
            if (visitCycle(g, v, color))
                return 1;
        }

        neighbor = neighbor->next;
    }

    color[node] = 2;  // Black: fully explored

    return 0;
}

// Check whether the graph contains a cycle
int hasCycle(Graph *g)
{
    if (g == NULL || g->maxNodes <= 0)
        return 0;

    int *color = calloc(g->maxNodes, sizeof(int));

    if (color == NULL)
        return 0;

    for (int i = 0; i < g->maxNodes; i++)
    {
        if (color[i] == 0)
        {
            if (visitCycle(g, i, color))
            {
                free(color);
                return 1;
            }
        }
    }

    free(color);
    return 0;
}
