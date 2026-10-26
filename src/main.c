
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "../headers/case.h"
#include "../headers/save_load.h"
#include "../headers/ranking.h"
#include "../headers/cycle.h"
#include "../headers/bfs.h"
#include "../headers/dfs.h"
#include "../headers/dijkstra.h"
#include "../headers/visualization.h"
#include "../headers/bfs_animation.h"

#define MAX_NODES 100
#define HASH_SIZE 100
#define SAVE_FILE "case.txt"

/* ---------- input helpers ---------- */

/* Discards the rest of the current input line. */
static void clearLine(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

/* Reads an integer and consumes the rest of the line.
   Returns 1 on success, 0 on bad input, -1 at end of input. */
static int readInt(const char *prompt, int *out)
{
    printf("%s", prompt);

    int result = scanf("%d", out);

    if (result == EOF)
        return -1;

    clearLine();

    return result == 1 ? 1 : 0;
}

/* Reads one whole line into buf (newline removed). */
static int readLine(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);

    if (fgets(buf, (int)size, stdin) == NULL)
        return 0;

    buf[strcspn(buf, "\n")] = '\0';

    return 1;
}

/* Reads one word (no spaces). */
static int readWord(const char *prompt, char *buf, size_t size)
{
    char fmt[16];

    snprintf(fmt, sizeof(fmt), "%%%zus", size - 1);

    printf("%s", prompt);

    if (scanf(fmt, buf) != 1)
    {
        clearLine();
        return 0;
    }

    clearLine();

    return 1;
}

/* Text that would break the '|' separated save file is rejected. */
static int hasPipe(const char *text)
{
    return strchr(text, '|') != NULL;
}

/* Prints a person's name, or Unknown(ID) if not found. */
static void printName(Case *c, int id)
{
    struct Person *p = searchPerson(c->people, id);

    if (p != NULL)
        printf("%s", p->name);
    else
        printf("Unknown(%d)", id);
}

/* ---------- menu options ---------- */

static void addPersonMenu(Case *c)
{
    struct Person p = {0};

    if (readInt("\nEnter person ID: ", &p.id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    if (p.id < 0 || p.id >= MAX_NODES)
    {
        printf("ID must be between 0 and %d.\n", MAX_NODES - 1);
        return;
    }

    if (searchPerson(c->people, p.id) != NULL)
    {
        printf("Person ID already exists.\n");
        return;
    }

    if (!readLine("Enter name: ", p.name, sizeof(p.name)) ||
        p.name[0] == '\0')
    {
        printf("Name cannot be empty.\n");
        return;
    }

    if (hasPipe(p.name))
    {
        printf("Name cannot contain '|'.\n");
        return;
    }

    if (!readLine("Enter role (suspect/witness/officer): ",
                  p.role, sizeof(p.role)))
    {
        printf("Could not read role.\n");
        return;
    }

    if (hasPipe(p.role))
    {
        printf("Role cannot contain '|'.\n");
        return;
    }

    if (readInt("Enter age: ", &p.age) != 1 || p.age < 0)
    {
        printf("Invalid age.\n");
        return;
    }

    if (!readLine("Enter contact (may be empty): ",
                  p.contact, sizeof(p.contact)) ||
        !readLine("Enter notes (may be empty): ",
                  p.notes, sizeof(p.notes)))
    {
        printf("Could not read contact or notes.\n");
        return;
    }

    if (hasPipe(p.contact) || hasPipe(p.notes))
    {
        printf("Contact and notes cannot contain '|'.\n");
        return;
    }

    if (addPerson(c, p))
        printf("Person added successfully.\n");
    else
        printf("Could not add person.\n");
}

static void searchPersonMenu(Case *c)
{
    int id;

    if (readInt("\nEnter person ID to search: ", &id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    struct Person *p = searchPerson(c->people, id);

    if (p == NULL)
    {
        printf("Person not found.\n");
        return;
    }

    printf("\nID: %d\nName: %s\nRole: %s\nAge: %d\nContact: %s\nNotes: %s\n",
           p->id, p->name, p->role, p->age, p->contact, p->notes);
}

/* True if name, reduced to lowercase letters, equals key. */
static int matchesTrieKey(const char *name, const char *key)
{
    while (*key != '\0')
    {
        if (!isalpha((unsigned char)*name))
        {
            if (*name == '\0')
                return 0;

            name++;
            continue;
        }

        if (tolower((unsigned char)*name) != *key)
            return 0;

        name++;
        key++;
    }

    while (*name != '\0')
    {
        if (isalpha((unsigned char)*name))
            return 0;

        name++;
    }

    return 1;
}

/* Shows the real name for a trie result, falling back to the key. */
static void printDisplayName(Case *c, const char *key)
{
    for (int i = 0; i < c->people->size; i++)
    {
        PersonNode *node = c->people->table[i];

        while (node != NULL)
        {
            if (matchesTrieKey(node->person.name, key))
            {
                printf("%s", node->person.name);
                return;
            }

            node = node->next;
        }
    }

    printf("%s", key);
}

static void autocompleteMenu(Case *c)
{
    char prefix[MAX_WORD_LENGTH];
    char results[10][MAX_WORD_LENGTH];

    if (!readWord("\nEnter the start of a name: ",
                  prefix, sizeof(prefix)))
    {
        printf("Invalid input.\n");
        return;
    }

    int count = trieAutocomplete(c->nameTrie, prefix, results, 10);

    if (count == 0)
    {
        printf("No names start with \"%s\".\n", prefix);
        return;
    }

    printf("Matches:\n");

    for (int i = 0; i < count; i++)
    {
        printf("  ");
        printDisplayName(c, results[i]);
        printf("\n");
    }
}

static void connectionMenu(Case *c)
{
    int a, b;

    if (readInt("\nFrom person ID: ", &a) != 1 ||
        readInt("To person ID: ", &b) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    if (searchPerson(c->people, a) == NULL ||
        searchPerson(c->people, b) == NULL)
    {
        printf("Both people must exist.\n");
        return;
    }

    BFSResult *r = bfs(c->graph, a, b);

    if (r == NULL)
    {
        printf("BFS failed: insufficient memory.\n");
        return;
    }

    if (r->pathLength == 0)
    {
        printf("No connection found.\n");
    }
    else
    {
        printf("Fewest-step connection (%d people): ", r->pathLength);

        for (int i = 0; i < r->pathLength; i++)
        {
            if (i > 0)
                printf(" -> ");

            printName(c, r->path[i]);
        }

        printf("\n");
    }

    freeBFSResult(r);
}

static void connectedMenu(Case *c)
{
    int id;

    if (readInt("\nPerson ID: ", &id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    if (searchPerson(c->people, id) == NULL)
    {
        printf("Person not found.\n");
        return;
    }

    DFSResult *r = dfs(c->graph, id);

    if (r == NULL)
    {
        printf("DFS failed: insufficient memory.\n");
        return;
    }

    printf("Reachable from %s (%d people, following relationship direction):\n",
           searchPerson(c->people, id)->name, r->count);

    for (int i = 0; i < r->count; i++)
    {
        printf("  ");
        printName(c, r->reachable[i]);
        printf("\n");
    }

    freeDFSResult(r);
}

static void routeMenu(Case *c)
{
    int a, b;

    if (readInt("\nFrom person ID: ", &a) != 1 ||
        readInt("To person ID: ", &b) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    if (searchPerson(c->people, a) == NULL ||
        searchPerson(c->people, b) == NULL)
    {
        printf("Both people must exist.\n");
        return;
    }

    DijkstraResult *r = dijkstra(c->graph, a, b);

    if (r == NULL)
    {
        printf("Dijkstra failed: insufficient memory.\n");
        return;
    }

    if (r->distance == INT_MAX)
    {
        printf("No route found.\n");
    }
    else
    {
        printf("Lowest-cost route: ");

        for (int i = 0; i < r->pathLength; i++)
        {
            if (i > 0)
                printf(" -> ");

            printName(c, r->path[i]);
        }

        printf("\nTotal cost: %d\n", r->distance);
    }

    freeDijkstraResult(r);
}

static void rankMenu(Case *c)
{
    int evidenceCount[MAX_NODES] = {0};

    for (int i = 0; i < c->evidenceCount; i++)
    {
        int pid = c->evidence[i].person_id;

        if (pid >= 0 && pid < MAX_NODES)
            evidenceCount[pid]++;
    }

    rankLeads(c->graph, evidenceCount, MAX_NODES);
}

static void cycleMenu(Case *c)
{
    printf("Cycle detected: %s\n",
           hasCycle(c->graph) ? "YES" : "NO");
}

static void relationshipMenu(Case *c)
{
    int src, dst, weight;
    char type[100];

    if (readInt("\nSource person ID: ", &src) != 1 ||
        readInt("Destination person ID: ", &dst) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    if (searchPerson(c->people, src) == NULL ||
        searchPerson(c->people, dst) == NULL)
    {
        printf("Both people must exist first.\n");
        return;
    }

    if (!readWord("Relationship type (e.g. CONTACT, VISITED): ",
                  type, sizeof(type)))
    {
        printf("Invalid type.\n");
        return;
    }

    if (readInt("Weight (cost or distance): ", &weight) != 1)
    {
        printf("Invalid weight.\n");
        return;
    }

    if (addEdge(c->graph, src, dst, type, weight))
        printf("Relationship added.\n");
    else
        printf("Could not add relationship.\n");
}

static void saveMenu(Case *c)
{
    if (saveCase(SAVE_FILE, c))
        printf("Case saved to %s.\n", SAVE_FILE);
    else
        printf("Failed to save case.\n");
}

/* Rebuilds the name trie from the people in the hash table. */
static void rebuildNameTrie(Case *c)
{
    for (int i = 0; i < c->people->size; i++)
    {
        PersonNode *node = c->people->table[i];

        while (node != NULL)
        {
            trieInsert(c->nameTrie, node->person.name);
            node = node->next;
        }
    }
}

/* Loads into a fresh case. The current case is replaced only on success. */
static void loadMenu(Case **cp)
{
    Case *fresh = createCase(HASH_SIZE, MAX_NODES);

    if (fresh == NULL)
    {
        printf("Out of memory.\n");
        return;
    }

    if (loadCase(SAVE_FILE, fresh))
    {
        rebuildNameTrie(fresh);
        freeCase(*cp);
        *cp = fresh;
        printf("Case loaded from %s.\n", SAVE_FILE);
    }
    else
    {
        freeCase(fresh);
        printf("Failed to load %s. Check the file format.\n", SAVE_FILE);
    }
}

static void undoMenu(Case *c)
{
    if (undoLast(c))
        printf("Last action undone.\n");
    else
        printf("Nothing to undo.\n");
}

/* ---------- graph visualization ---------- */

static void visualizeGraphMenu(Case *c)
{
    if (c == NULL || c->graph == NULL)
    {
        printf("Graph is not initialized.\n");
        return;
    }

    if (exportGraphviz(c->graph, "case_graph.dot", "case_graph.png"))
    {
        printf("Graph exported successfully!\n");
        printf("DOT file: case_graph.dot\n");
        printf("Image: case_graph.png\n");
        printf("Open case_graph.png in the VS Code Explorer.\n");
    }
    else
    {
        printf("Failed to generate graph image.\n");
        printf("Check Graphviz installation and file permissions.\n");
    }
}
static void bfsAnimationMenu(Case *c)
{
    int source;
    int target;

    printf("Enter source person ID: ");
    if (scanf("%d", &source) != 1)
        return;

    printf("Enter target person ID: ");
    if (scanf("%d", &target) != 1)
        return;

    animateBFS(c->graph, source, target);
}

static void printMenu(void)
{
    printf("\n========== CRIME INVESTIGATION SYSTEM ==========\n");
    printf("1. Add person\n");
    printf("2. Search person by ID\n");
    printf("3. Autocomplete name\n");
    printf("4. Find connection (fewest steps)\n");
    printf("5. Show connected people\n");
    printf("6. Shortest route (lowest cost)\n");
    printf("7. Rank leads\n");
    printf("8. Check for cycles\n");
    printf("9. Save case\n");
    printf("10. Load case\n");
    printf("11. Add relationship\n");
    printf("12. Undo last action\n");
    printf("13. Visualize investigation graph\n");
    printf("14. Animate BFS\n");
    printf("0. Exit\n");
}

int main(void)
{
    Case *c = createCase(HASH_SIZE, MAX_NODES);

    if (c == NULL)
    {
        printf("Failed to initialize the system.\n");
        return 1;
    }

    int choice = -1;

    do
    {
        printMenu();

        int r = readInt("Enter your choice: ", &choice);

        if (r == -1)
            break;

        if (r == 0)
        {
            printf("Invalid input. Enter a number.\n");
            continue;
        }

        switch (choice)
        {
            case 1:
                addPersonMenu(c);
                break;

            case 2:
                searchPersonMenu(c);
                break;

            case 3:
                autocompleteMenu(c);
                break;

            case 4:
                connectionMenu(c);
                break;

            case 5:
                connectedMenu(c);
                break;

            case 6:
                routeMenu(c);
                break;

            case 7:
                rankMenu(c);
                break;

            case 8:
                cycleMenu(c);
                break;

            case 9:
                saveMenu(c);
                break;

            case 10:
                loadMenu(&c);
                break;

            case 11:
                relationshipMenu(c);
                break;

            case 12:
                undoMenu(c);
                break;

            case 13:
                visualizeGraphMenu(c);
                break;

            case 14:
                bfsAnimationMenu(c);
                break;

            case 0:
                printf("Exiting. Goodbye!\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 0);

    freeCase(c);

    return 0;
}
