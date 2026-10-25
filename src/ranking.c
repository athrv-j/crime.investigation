#include <stdio.h>
#include <stdlib.h>

#include "../headers/ranking.h"
#include "../headers/heap.h"

/* score = outgoing relationships * 10 + linked evidence * 20 */
int scoreLead(int edgeCount, int evidenceCount)
{
    return edgeCount * 10 + evidenceCount * 20;
}

/* Print every registered person, highest score first.
   Empty ID slots (never added) are skipped. */
void rankLeads(Graph *g, int *evidenceCount, int n)
{
    if (g == NULL || evidenceCount == NULL || n <= 0)
        return;

    if (n > g->maxNodes)
        n = g->maxNodes;

    MaxHeap *heap = createHeap(n);
    if (heap == NULL)
    {
        printf("Failed to create ranking heap.\n");
        return;
    }

    int added = 0;
    for (int i = 0; i < n; i++)
    {
        if (g->nodeExists[i] == 0)
            continue;

        int edgeCount = 0;
        EdgeNode *neighbor = getNeighbors(g, i);
        while (neighbor != NULL)
        {
            edgeCount++;
            neighbor = neighbor->next;
        }

        Lead lead;
        lead.personId = i;
        lead.score = scoreLead(edgeCount, evidenceCount[i]);

        if (!heapInsert(heap, lead))
        {
            printf("Failed to insert lead %d.\n", i);
            freeHeap(heap);
            return;
        }
        added++;
    }

    if (added == 0)
    {
        printf("No people to rank yet.\n");
        freeHeap(heap);
        return;
    }

    printf("Lead ranking (highest score first):\n");
    while (heap->size > 0)
    {
        Lead lead = heapExtractMax(heap);
        if (lead.personId == -1)
            break;
        printf("Person %d | Score: %d\n", lead.personId, lead.score);
    }

    freeHeap(heap);
}
