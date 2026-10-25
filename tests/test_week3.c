
#include <stdio.h>

#include "../headers/graph.h"
#include "../headers/cycle.h"
#include "../headers/ranking.h"

int main(void)
{
    Graph *g = createGraph(3);

    if (g == NULL)
    {
        printf("Graph creation failed!\n");
        return 1;
    }

    // Test 1: Create a graph without a cycle
    addEdge(g, 0, 1, "contact", 1);
    addEdge(g, 1, 2, "contact", 1);

    int result = hasCycle(g);

    printf("Test 1 - No cycle: %s\n",
           result == 0 ? "PASS" : "FAIL");

    // Test 2: Add an edge to create a cycle
    addEdge(g, 2, 0, "contact", 1);

    result = hasCycle(g);

    printf("Test 2 - Cycle detected: %s\n",
           result == 1 ? "PASS" : "FAIL");

    freeGraph(g);

    // Test 3: Create a separate graph for ranking
    Graph *rankingGraph = createGraph(3);

    if (rankingGraph == NULL)
    {
        printf("Ranking graph creation failed!\n");
        return 1;
    }

    addEdge(rankingGraph, 0, 1, "contact", 1);
    addEdge(rankingGraph, 0, 2, "contact", 1);
    addEdge(rankingGraph, 2, 1, "contact", 1);

    int evidenceCount[] = {0, 3, 0};

    printf("\nTest 3 - Lead ranking:\n");
    rankLeads(rankingGraph, evidenceCount, 3);

    freeGraph(rankingGraph);

    return 0;
}
