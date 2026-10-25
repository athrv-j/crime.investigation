
#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode
{
    char task[200];
    struct QueueNode* next;
} QueueNode;

typedef struct
{
    QueueNode* front;
    QueueNode* rear;
} Queue;

// Create an empty queue
Queue* createQueue(void);

// Add a task to the queue
int enqueue(Queue* q, const char* task);

// Remove the oldest task
int dequeue(Queue* q, char output[]);

// Check whether the queue is empty
int isQueueEmpty(const Queue* q);

// Free all queue memory
void freeQueue(Queue* q);

#endif
