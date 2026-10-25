
#include <stdlib.h>
#include <string.h>
#include "../headers/queue.h"

// Create an empty queue
Queue* createQueue(void)
{
    Queue* q = malloc(sizeof(Queue));

    if (q == NULL)
        return NULL;

    q->front = NULL;
    q->rear = NULL;

    return q;
}

// Add a task to the rear
int enqueue(Queue* q, const char* task)
{
    if (q == NULL || task == NULL)
        return 0;

    size_t length = strlen(task);

    // Ensure the task fits inside the node
    if (length >= sizeof(((QueueNode*)0)->task))
        return 0;

    QueueNode* newNode = malloc(sizeof(QueueNode));

    if (newNode == NULL)
        return 0;

    memcpy(newNode->task, task, length + 1);
    newNode->next = NULL;

    if (q->rear == NULL)
    {
        // Queue is empty
        q->front = newNode;
        q->rear = newNode;
    }
    else
    {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    return 1;
}

// Remove the task at the front
int dequeue(Queue* q, char output[])
{
    if (q == NULL || q->front == NULL || output == NULL)
        return 0;

    QueueNode* temp = q->front;

    strcpy(output, temp->task);

    q->front = q->front->next;

    // Queue became empty
    if (q->front == NULL)
        q->rear = NULL;

    free(temp);

    return 1;
}

// Check if queue is empty
int isQueueEmpty(const Queue* q)
{
    return q == NULL || q->front == NULL;
}

// Free the entire queue
void freeQueue(Queue* q)
{
    if (q == NULL)
        return;

    while (!isQueueEmpty(q))
    {
        QueueNode* temp = q->front;
        q->front = q->front->next;
        free(temp);
    }

    free(q);
}
