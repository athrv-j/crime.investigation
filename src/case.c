#include <stdlib.h>
#include "../headers/case.h"

Case* createCase(int hashSize, int maxNodes)
{
    if (hashSize <= 0 || maxNodes <= 0)
        return NULL;

    Case *c = calloc(1, sizeof(Case));
    if (c == NULL)
        return NULL;

    c->people    = createHashTable(hashSize);
    c->graph     = createGraph(maxNodes);
    c->nameTrie  = createTrieNode();
    c->undoStack = createStack(UNDO_CAPACITY);
    c->tasks     = createQueue();

    if (c->people == NULL || c->graph == NULL || c->nameTrie == NULL ||
        c->undoStack == NULL || c->tasks == NULL)
    {
        freeCase(c);
        return NULL;
    }
    return c;
}

void freeCase(Case *c)
{
    if (c == NULL)
        return;

    freeHashTable(c->people);
    freeGraph(c->graph);
    freeTrie(c->nameTrie);

    if (c->undoStack != NULL)
    {
        while (!isEmpty(c->undoStack))
            free(pop(c->undoStack));
        freeStack(c->undoStack);
    }

    freeQueue(c->tasks);
    free(c);
}

int addPerson(Case *c, struct Person p)
{
    if (c == NULL || p.id < 0 || p.id >= c->graph->maxNodes)
        return 0;

    if (searchPerson(c->people, p.id) != NULL)
        return 0;

    insertPerson(c->people, p);
    addNode(c->graph, p.id);
    trieInsert(c->nameTrie, p.name);

    /* Record the action for undo, if there is room on the stack */
    if (c->undoStack->top < c->undoStack->capacity - 1)
    {
        UndoRecord *rec = malloc(sizeof(UndoRecord));
        if (rec != NULL)
        {
            rec->type = UNDO_ADD_PERSON;
            rec->personId = p.id;
            push(c->undoStack, rec);
        }
    }
    return 1;
}

int undoLast(Case *c)
{
    if (c == NULL || isEmpty(c->undoStack))
        return 0;

    UndoRecord *rec = pop(c->undoStack);

    if (rec->type == UNDO_ADD_PERSON)
    {
        deletePerson(c->people, rec->personId);

        /* Unregister the node so ranking skips it.
           Known limitation: the name stays in the trie and any
           relationships stay in the graph. */
        if (c->graph->nodeExists[rec->personId])
        {
            c->graph->nodeExists[rec->personId] = 0;
            c->graph->nodeCount--;
        }
    }

    free(rec);
    return 1;
}

int addLocation(Case *c, struct Location location)
{
    if (c == NULL || c->locationCount >= MAX_LOCATIONS)
        return 0;

    for (int i = 0; i < c->locationCount; i++)
    {
        if (c->locations[i].id == location.id)
            return 0;
    }

    c->locations[c->locationCount++] = location;
    return 1;
}

int addEvidence(Case *c, struct Evidence evidence)
{
    if (c == NULL || c->evidenceCount >= MAX_EVIDENCE)
        return 0;

    for (int i = 0; i < c->evidenceCount; i++)
    {
        if (c->evidence[i].id == evidence.id)
            return 0;
    }

    c->evidence[c->evidenceCount++] = evidence;
    return 1;
}

int addEvent(Case *c, struct Event event)
{
    if (c == NULL || c->eventCount >= MAX_EVENTS)
        return 0;

    for (int i = 0; i < c->eventCount; i++)
    {
        if (c->events[i].id == event.id)
            return 0;
    }

    c->events[c->eventCount++] = event;
    return 1;
}
