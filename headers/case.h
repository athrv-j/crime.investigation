#ifndef CASE_H
#define CASE_H

#include "hash_table.h"
#include "graph.h"
#include "trie.h"
#include "stack.h"
#include "queue.h"

#define MAX_LOCATIONS 100
#define MAX_EVIDENCE  100
#define MAX_EVENTS    100
#define UNDO_CAPACITY 100

/* What an undo record refers to. */
typedef enum {
    UNDO_ADD_PERSON = 1
} UndoType;

typedef struct {
    UndoType type;
    int personId;
} UndoRecord;

/* One investigation: all the data structures in one object. */
typedef struct {
    HashTable *people;          /* person lookup by ID */
    Graph     *graph;           /* relationships */
    TrieNode  *nameTrie;        /* name autocomplete */
    Stack     *undoStack;       /* UndoRecord* values */
    Queue     *tasks;           /* pending investigation tasks */

    struct Location locations[MAX_LOCATIONS];
    int locationCount;

    struct Evidence evidence[MAX_EVIDENCE];
    int evidenceCount;

    struct Event events[MAX_EVENTS];
    int eventCount;
} Case;

Case* createCase(int hashSize, int maxNodes);   /* NULL on failure */
void  freeCase(Case *c);

/* Adds a person to the hash table, graph, trie, and undo stack.
   Returns 1 on success, 0 on failure (bad ID, duplicate, out of memory). */
int   addPerson(Case *c, struct Person p);

/* Reverses the most recent undoable action. Returns 1 if something was undone. */
int   undoLast(Case *c);

int   addLocation(Case *c, struct Location location);
int   addEvidence(Case *c, struct Evidence evidence);
int   addEvent(Case *c, struct Event event);

#endif
