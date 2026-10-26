
#include <stdio.h>
#include <stdlib.h>
#include "visualization.h"


int exportGraphviz(const Graph *g, const char *dotFile,
                   const char *pngFile)
{
    if (g == NULL || dotFile == NULL || pngFile == NULL)
        return 0;

    FILE *file = fopen(dotFile, "w");

    if (file == NULL)
        return 0;

    fprintf(file, "digraph Investigation {\n");
    fprintf(file, "    rankdir=LR;\n");
    fprintf(file, "    bgcolor=\"white\";\n");
    fprintf(file, "    node [shape=box, style=filled, "
                   "fillcolor=lightblue];\n");
    fprintf(file, "    edge [fontname=\"Arial\"];\n");

    /* Write all registered people as graph nodes. */
    for (int i = 0; i < g->maxNodes; i++)
    {
        if (g->nodeExists[i])
        {
            fprintf(file, "    \"Person %d\";\n", i);
        }
    }

    /* Write directed relationships and their weights. */
    for (int i = 0; i < g->maxNodes; i++)
    {
        if (!g->nodeExists[i])
            continue;

        EdgeNode *edge = g->adjacencyList[i];

        while (edge != NULL)
        {
            fprintf(file,
                    "    \"Person %d\" -> \"Person %d\" "
                    "[label=\"%s | weight %d\"];\n",
                    i,
                    edge->destinationId,
                    edge->type,
                    edge->weight);

            edge = edge->next;
        }
    }

    fprintf(file, "}\n");

    if (fclose(file) != 0)
        return 0;

    char command[512];

    int written = snprintf(
        command,
        sizeof(command),
        "dot -Tpng \"%s\" -o \"%s\"",
        dotFile,
        pngFile
    );

    if (written < 0 || (size_t)written >= sizeof(command))
        return 0;

    return system(command) == 0;
}
