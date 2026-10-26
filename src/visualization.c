
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
    fprintf(file, "    node [shape=box, style=filled, "
                   "fillcolor=lightblue];\n");

    /*
     * Next, we will export the actual nodes and edges
     * using the exact fields in your graph.h.
     */

    fprintf(file, "}\n");

    fclose(file);

    char command[512];
    int written = snprintf(command, sizeof(command),
                           "dot -Tpng \"%s\" -o \"%s\"",
                           dotFile, pngFile);

    if (written < 0 || (size_t)written >= sizeof(command))
        return 0;

    return system(command) == 0;
}
