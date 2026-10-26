
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../headers/dfs_animation.h"

static int renderDFSFrame(
    Graph *g,
    const int *visited,
    int current,
    int source,
    int step
)
{
    char dotFile[128];
    char pngFile[128];

    snprintf(dotFile, sizeof(dotFile),
             "dfs_frames/frame_%04d.dot", step);

    snprintf(pngFile, sizeof(pngFile),
             "dfs_frames/frame_%04d.png", step);

    FILE *file = fopen(dotFile, "w");

    if (file == NULL)
        return 0;

    fprintf(file, "digraph DFS {\n");
    fprintf(file, "rankdir=LR;\n");
    fprintf(file, "label=\"DFS Step %d\";\n", step);
    fprintf(file, "labelloc=t;\n");
    fprintf(file, "node [shape=circle, style=filled];\n");

    for (int i = 0; i < g->maxNodes; i++)
    {
        if (!g->nodeExists[i])
            continue;

        const char *color = "white";

        if (i == current)
            color = "gold";
        else if (i == source)
            color = "palegreen";
        else if (visited[i])
            color = "lightblue";

        fprintf(file,
                "\"%d\" [label=\"Person %d\", fillcolor=\"%s\"];\n",
                i, i, color);
    }

    for (int i = 0; i < g->maxNodes; i++)
    {
        if (!g->nodeExists[i])
            continue;

        EdgeNode *edge = g->adjacencyList[i];

        while (edge != NULL)
        {
            fprintf(file, "\"%d\" -> \"%d\" "
                    "[label=\"%s (%d)\"];\n",
                    i, edge->destinationId,
                    edge->type, edge->weight);

            edge = edge->next;
        }
    }

    fprintf(file, "}\n");

    if (fclose(file) != 0)
        return 0;

    char command[512];

    int written = snprintf(
        command, sizeof(command),
        "dot -Tpng \"%s\" -o \"%s\"",
        dotFile, pngFile
    );

    if (written < 0 || (size_t)written >= sizeof(command))
        return 0;

    return system(command) == 0;
}

static int dfsVisit(
    Graph *g,
    int node,
    int *visited,
    int source,
    int *step
)
{
    visited[node] = 1;

    if (!renderDFSFrame(g, visited, node, source, (*step)++))
        return 0;

    EdgeNode *edge = getNeighbors(g, node);

    while (edge != NULL)
    {
        int next = edge->destinationId;

        if (next >= 0 && next < g->maxNodes &&
            g->nodeExists[next] && !visited[next])
        {
            if (!dfsVisit(g, next, visited, source, step))
                return 0;
        }

        edge = edge->next;
    }

    return renderDFSFrame(g, visited, node, source, (*step)++);
}

int animateDFS(Graph *g, int source)
{
    if (g == NULL || source < 0 ||
        source >= g->maxNodes || !g->nodeExists[source])
    {
        printf("Invalid DFS source ID.\n");
        return 0;
    }

    if (mkdir("dfs_frames", 0777) != 0)
    {
        FILE *test = fopen("dfs_frames/.write_test", "w");

        if (test == NULL)
        {
            perror("Cannot access dfs_frames");
            return 0;
        }

        fclose(test);
        remove("dfs_frames/.write_test");
    }

    int *visited = calloc((size_t)g->maxNodes, sizeof(int));

    if (visited == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }

    int step = 0;
    int ok = dfsVisit(g, source, visited, source, &step);

    free(visited);

    if (!ok)
    {
        printf("Failed to generate DFS frames.\n");
        return 0;
    }

    char command[512];

    int written = snprintf(
        command, sizeof(command),
        "ffmpeg -y -loglevel error -framerate 1 "
        "-i dfs_frames/frame_%%04d.png "
        "-vf \"scale=900:-1:flags=lanczos,"
        "split[s0][s1];[s0]palettegen[p];"
        "[s1][p]paletteuse\" "
        "-loop 0 dfs_animation.gif"
    );

    if (written < 0 || (size_t)written >= sizeof(command))
        return 0;

    if (system(command) != 0)
    {
        printf("FFmpeg failed to create the GIF.\n");
        return 0;
    }

    printf("DFS animation created: dfs_animation.gif\n");
    printf("Frames saved in dfs_frames/.\n");

    return 1;
}
