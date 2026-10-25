
#include <stdlib.h>
#include "../headers/timeline.h"

int compareEvents(const void *a, const void *b)
{
    const struct Event *eventA = a;
    const struct Event *eventB = b;

    if (eventA->timespan < eventB->timespan)
        return -1;

    if (eventA->timespan > eventB->timespan)
        return 1;

    return 0;
}

void sortEvents(struct Event events[], int count)
{
    if (events == NULL || count <= 1)
        return;

    qsort(events, (size_t)count, sizeof(struct Event),
          compareEvents);
}
