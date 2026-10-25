
#include <stdio.h>
#include "graph.h"
#include "cycle.h"

int main(void)
{
    Graph *g = createGraph(3);

    if (g == NULL)
    {
        printf("Graph creation failed!\n");
        return 1;
    }

    // Create the path 0 -> 1 -> 2
    addNode(g, 0);
    addNode(g, 1);
    addNode(g, 2);

    addEdge(g, 0, 1, "contact", 1);
    addEdge(g, 1, 2, "contact", 1);

    printf("Without cycle: %d\n", hasCycle(g));

    // Add the edge 2 -> 0 to create a cycle
    addEdge(g, 2, 0, "contact", 1);

    printf("With cycle: %d\n", hasCycle(g));

    freeGraph(g);

    return 0;
}
