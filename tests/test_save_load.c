
#include <stdio.h>
#include <string.h>
#include "../headers/save_load.h"

#define HASH_SIZE 100
#define MAX_NODES 100

int main(void)
{
    Case *c = createCase(HASH_SIZE, MAX_NODES);

    if (c == NULL)
    {
        printf("Failed to create case.\n");
        return 1;
    }

    /* Add five people */
    struct Person people[5] = {
        {1, "Rahul", "suspect", 28, "9876543210", "Main suspect"},
        {2, "Aditya", "witness", 25, "9876543211", "Saw the incident"},
        {3, "Priya", "victim", 22, "9876543212", "Victim record"},
        {4, "Rohan", "witness", 30, "9876543213", "Nearby witness"},
        {5, "Sneha", "suspect", 27, "9876543214", "Under investigation"}
    };

    for (int i = 0; i < 5; i++)
    {
        insertPerson(c->people, people[i]);
        addNode(c->graph, people[i].id);
    }

    /* Add a relationship */
    addEdge(c->graph, 1, 2, "CONTACT", 5);

    /* Save the case */
    if (!saveCase("test_case.txt", c))
    {
        printf("FAIL: Could not save case.\n");
        freeCase(c);
        return 1;
    }

    printf("Save test: PASS\n");

    /* Create a fresh case to load into */
    Case *loaded = createCase(HASH_SIZE, MAX_NODES);

    if (loaded == NULL)
    {
        printf("Failed to create loading case.\n");
        freeCase(c);
        return 1;
    }

    if (!loadCase("test_case.txt", loaded))
    {
        printf("FAIL: Could not load case.\n");
        freeCase(c);
        freeCase(loaded);
        return 1;
    }

    printf("Load test: PASS\n");

    /* Verify all five people */
    int passed = 1;

    for (int i = 0; i < 5; i++)
    {
        struct Person *p = searchPerson(loaded->people, people[i].id);

        if (p == NULL || strcmp(p->name, people[i].name) != 0)
        {
            printf("FAIL: Person %d was not loaded correctly.\n",
                   people[i].id);
            passed = 0;
        }
    }

    if (passed)
    {
        printf("Five-person load test: PASS\n");
    }

    /* Verify the graph relationship */
    EdgeNode *edge = getNeighbors(loaded->graph, 1);

    if (edge != NULL &&
        edge->destinationId == 2 &&
        strcmp(edge->type, "CONTACT") == 0 &&
        edge->weight == 5)
    {
        printf("Graph edge load test: PASS\n");
    }
    else
    {
        printf("Graph edge load test: FAIL\n");
        passed = 0;
    }

    freeCase(c);
    freeCase(loaded);

    if (passed)
    {
        printf("\nALL SAVE/LOAD TESTS PASSED.\n");
        return 0;
    }

    printf("\nSOME SAVE/LOAD TESTS FAILED.\n");
    return 1;
}
