
#include <stdio.h>
#include "../headers/timeline.h"

int main(void)
{
    struct Event events[4] = {
        {.id = 1, .timespan = 300},
        {.id = 2, .timespan = 100},
        {.id = 3, .timespan = 400},
        {.id = 4, .timespan = 200}
    };

    sortEvents(events, 4);

    time_t expected[] = {100, 200, 300, 400};

    for (int i = 0; i < 4; i++)
    {
        if (events[i].timespan != expected[i])
        {
            printf("Timeline test: FAIL\n");
            return 1;
        }
    }

    printf("Timeline test: PASS\n");
    return 0;
}
