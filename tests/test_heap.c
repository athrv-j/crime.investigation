
#include <stdio.h>
#include "headers/heap.h"

int main(void)
{
    MaxHeap* heap = createHeap(10);

    if (heap == NULL)
    {
        printf("Heap creation failed!\n");
        return 1;
    }

    int scores[] = {5, 1, 9, 3, 7, 2};
    int n = sizeof(scores) / sizeof(scores[0]);

    for (int i = 0; i < n; i++)
    {
        Lead lead = {i + 1, scores[i]};

        if (!heapInsert(heap, lead))
        {
            printf("Insertion failed!\n");
            freeHeap(heap);
            return 1;
        }
    }

    printf("Extracted scores: ");

    for (int i = 0; i < n; i++)
    {
        Lead lead = heapExtractMax(heap);
        printf("%d ", lead.score);
    }

    printf("\n");

    freeHeap(heap);

    return 0;
}
