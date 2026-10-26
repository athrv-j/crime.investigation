
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../headers/bfs_animation.h"

static int renderFrame(
    Graph *g,
    const int *visited,
    const int *parent,
    int current,
    int source,
    int target,
    int frame
)
{
    char dotFile[128];
    char pngFile[128];

    snprintf(dotFile, sizeof(dotFile),
             "bfs_frames/frame_%04d.dot", frame);

    snprintf(pngFile, sizeof(pngFile),
             "bfs_frames/frame_%04d.png", frame);

    FILE *file = fopen(dotFile, "w");

    if (file == NULL)
        return 0;

    fprintf(file, "digraph BFS {\n");
    fprintf(file, "rankdir=LR;\n");
    fprintf(file, "label=\"BFS Step %d\";\n", frame);
    fprintf(file, "labelloc=t;\n");
    fprintf(file, "node [shape=circle, style=filled, "
                  "fontname=\"Arial\"];\n");

    for (int i = 0; i < g->maxNodes; i++)
    {
        if (!g->nodeExists[i])
            continue;

        const char *color = "white";

        if (i == current)
            color = "gold";
        else if (i == source)
            color = "palegreen";
        else if (i == target)
            color = "lightpink";
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
            int v = edge->destinationId;
            const char *color = "gray";
            int penwidth = 1;

            if (parent[v] == i && visited[v])
            {
                color = "dodgerblue";
                penwidth = 3;
            }

            fprintf(file,
                    "\"%d\" -> \"%d\" "
                    "[label=\"%s (%d)\", color=\"%s\", "
                    "penwidth=%d];\n",
                    i, v, edge->type, edge->weight,
                    color, penwidth);

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

int animateBFS(Graph *g, int source, int target)
{
    if (g == NULL ||
        source < 0 || source >= g->maxNodes ||
        target < 0 || target >= g->maxNodes ||
        !g->nodeExists[source] || !g->nodeExists[target])
    {
        printf("Invalid BFS source or target.\n");
        return 0;
    }

    if (mkdir("bfs_frames", 0777) != 0)
    {
        /* The directory may already exist. */
        FILE *test = fopen("bfs_frames/.write_test", "w");

        if (test == NULL)
        {
            perror("Cannot access bfs_frames");
            return 0;
        }

        fclose(test);
        remove("bfs_frames/.write_test");
    }

    int *queue = malloc((size_t)g->maxNodes * sizeof(int));
    int *visited = calloc((size_t)g->maxNodes, sizeof(int));
    int *parent = malloc((size_t)g->maxNodes * sizeof(int));

    if (queue == NULL || visited == NULL || parent == NULL)
    {
        free(queue);
        free(visited);
        free(parent);
        printf("Memory allocation failed.\n");
        return 0;
    }

    for (int i = 0; i < g->maxNodes; i++)
        parent[i] = -1;

    int front = 0;
    int rear = 0;
    int frame = 0;
    int found = 0;
    int ok = 1;

    queue[rear++] = source;
    visited[source] = 1;

    ok = renderFrame(g, visited, parent, source,
                     source, target, frame++);

    while (ok && front < rear)
    {
        int current = queue[front++];

        if (!renderFrame(g, visited, parent, current,
                         source, target, frame++))
        {
            ok = 0;
            break;
        }

        if (current == target)
        {
            found = 1;
            break;
        }

        EdgeNode *edge = getNeighbors(g, current);

        while (edge != NULL)
        {
            int next = edge->destinationId;

            if (next >= 0 && next < g->maxNodes &&
                g->nodeExists[next] && !visited[next])
            {
                visited[next] = 1;
                parent[next] = current;
                queue[rear++] = next;

                if (!renderFrame(g, visited, parent, current,
                                 source, target, frame++))
                {
                    ok = 0;
                    break;
                }
            }

            edge = edge->next;
        }
    }

    if (ok && !renderFrame(g, visited, parent, -1,
                           source, target, frame++))
        ok = 0;

    free(queue);
    free(visited);
    free(parent);

    if (!ok)
    {
        printf("Failed to generate BFS frames.\n");
        return 0;
    }

    if (!found)
        printf("No path exists between %d and %d.\n",
               source, target);

    char command[512];

    int written = snprintf(
        command, sizeof(command),
        "ffmpeg -y -loglevel error -framerate 1 "
        "-i bfs_frames/frame_%%04d.png "
        "-vf \"scale=900:-1:flags=lanczos,"
        "split[s0][s1];[s0]palettegen[p];"
        "[s1][p]paletteuse\" "
        "-loop 0 bfs_animation.gif"
    );

    if (written < 0 || (size_t)written >= sizeof(command))
        return 0;

    if (system(command) != 0)
    {
        printf("FFmpeg failed to create the GIF.\n");
        return 0;
    }

    printf("BFS animation created: bfs_animation.gif\n");
    printf("Frames saved in: bfs_frames/\n");

    return 1;
}
