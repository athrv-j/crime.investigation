
#include <stdio.h>

#include "../headers/graph.h"
#include "../headers/ranking.h"

int main(void)
{
    Graph *g = createGraph(3);

    if (g == NULL)
    {
        printf("Graph creation failed.\n");
        return 1;
    }

    // Person 0 has two outgoing edges
    addEdge(g, 0, 1, "contact", 1);
    addEdge(g, 0, 2, "contact", 1);

    // Person 2 has one outgoing edge
    addEdge(g, 2, 1, "contact", 1);

    int evidenceCount[] = {0, 3, 0};

    rankLeads(g, evidenceCount, 3);

    freeGraph(g);

    return 0;
}
