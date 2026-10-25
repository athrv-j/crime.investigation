
#include <stdio.h>
#include <stdlib.h>
#include "../headers/heap.h"

// Create a heap
MaxHeap* createHeap(int capacity)
{
    if (capacity <= 0)
        return NULL;

    MaxHeap* heap = malloc(sizeof(MaxHeap));

    if (heap == NULL)
        return NULL;

    heap->items = malloc(capacity * sizeof(Lead));

    if (heap->items == NULL)
    {
        free(heap);
        return NULL;
    }

    heap->size = 0;
    heap->capacity = capacity;

    return heap;
}

// Insert a lead into the max-heap
int heapInsert(MaxHeap* heap, Lead lead)
{
    if (heap == NULL || heap->size >= heap->capacity)
        return 0;

    int i = heap->size;
    heap->items[i] = lead;
    heap->size++;

    // Bubble up
    while (i > 0)
    {
        int parent = (i - 1) / 2;

        if (heap->items[i].score <= heap->items[parent].score)
            break;

        Lead temp = heap->items[i];
        heap->items[i] = heap->items[parent];
        heap->items[parent] = temp;

        i = parent;
    }

    return 1;
}

// Extract the highest-scoring lead
Lead heapExtractMax(MaxHeap* heap)
{
    Lead emptyLead = {-1, 0};

    if (heap == NULL || heap->size == 0)
        return emptyLead;

    Lead maxLead = heap->items[0];

    // Move the last lead to the root
    heap->items[0] = heap->items[heap->size - 1];
    heap->size--;

    // Bubble down
    int i = 0;

    while (i < heap->size)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;

        if (left < heap->size &&
            heap->items[left].score > heap->items[largest].score)
        {
            largest = left;
        }

        if (right < heap->size &&
            heap->items[right].score > heap->items[largest].score)
        {
            largest = right;
        }

        if (largest == i)
            break;

        Lead temp = heap->items[i];
        heap->items[i] = heap->items[largest];
        heap->items[largest] = temp;

        i = largest;
    }

    return maxLead;
}

// Free the heap
void freeHeap(MaxHeap* heap)
{
    if (heap == NULL)
        return;

    free(heap->items);
    free(heap);
}
