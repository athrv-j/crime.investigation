
#include <stdio.h>
#include "headers/queue.h"

int main(void)
{
    Queue* q = createQueue();

    if (q == NULL)
    {
        printf("Queue creation failed!\n");
        return 1;
    }

    enqueue(q, "A");
    enqueue(q, "B");
    enqueue(q, "C");

    char task[200];

    printf("Dequeued tasks:\n");

    while (!isQueueEmpty(q))
    {
        if (dequeue(q, task))
            printf("%s\n", task);
    }

    freeQueue(q);

    return 0;
}
