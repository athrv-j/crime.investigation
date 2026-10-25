
#ifndef HEAP_H
#define HEAP_H

typedef struct
{
    int personId;
    int score;
} Lead;

typedef struct
{
    Lead *items;
    int size;
    int capacity;
} MaxHeap;

// Create a heap with the given capacity
MaxHeap* createHeap(int capacity);

// Insert a lead into the heap
int heapInsert(MaxHeap* heap, Lead lead);

// Extract the lead with the highest score
Lead heapExtractMax(MaxHeap* heap);

// Free the heap
void freeHeap(MaxHeap* heap);

#endif
