
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../headers/dijkstra_animation.h"

static int renderDijkstraFrame(
    Graph *g,
    const int *distance,
    const int *visited,
    const int *parent,
    const int *finalPath,
    int current,
    int source,
    int target,
    int step
)
{
    char dotFile[128];
    char pngFile[128];

    snprintf(dotFile, sizeof(dotFile),
             "dijkstra_frames/frame_%04d.dot", step);
    snprintf(pngFile, sizeof(pngFile),
             "dijkstra_frames/frame_%04d.png", step);

    FILE *file = fopen(dotFile, "w");

    if (file == NULL)
        return 0;

    fprintf(file, "digraph Dijkstra {\n");
    fprintf(file, "rankdir=LR;\n");
    fprintf(file, "label=\"Dijkstra - Step %d\";\n", step);
    fprintf(file, "labelloc=t;\n");
    fprintf(file, "node [shape=circle, style=filled, "
                  "fontname=\"Arial\"];\n");

    for (int i = 0; i < g->maxNodes; i++)
    {
        if (!g->nodeExists[i])
            continue;

        const char *color = "white";

        if (finalPath != NULL && finalPath[i])
            color = "palegreen";
        else if (i == current)
            color = "gold";
        else if (i == source)
            color = "lightcyan";
        else if (visited[i])
            color = "lightblue";
        else if (i == target)
            color = "lightpink";

        if (distance[i] == INT_MAX)
        {
            fprintf(file,
                    "\"%d\" [label=\"Person %d\\nDist: INF\", "
                    "fillcolor=\"%s\"];\n",
                    i, i, color);
        }
        else
        {
            fprintf(file,
                    "\"%d\" [label=\"Person %d\\nDist: %d\", "
                    "fillcolor=\"%s\"];\n",
                    i, i, distance[i], color);
        }
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
            int width = 1;

            if (v >= 0 && v < g->maxNodes &&
                finalPath != NULL && finalPath[i] &&
                finalPath[v] && parent[v] == i)
            {
                color = "green4";
                width = 4;
            }

            fprintf(file,
                    "\"%d\" -> \"%d\" "
                    "[label=\"%s (%d)\", color=\"%s\", "
                    "penwidth=%d];\n",
                    i, v, edge->type, edge->weight,
                    color, width);

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

int animateDijkstra(Graph *g, int source, int target)
{
    if (g == NULL ||
        source < 0 || source >= g->maxNodes ||
        target < 0 || target >= g->maxNodes ||
        !g->nodeExists[source] || !g->nodeExists[target])
    {
        printf("Invalid Dijkstra source or target.\n");
        return 0;
    }

    if (mkdir("dijkstra_frames", 0777) != 0)
    {
        FILE *test = fopen("dijkstra_frames/.write_test", "w");

        if (test == NULL)
        {
            perror("Cannot access dijkstra_frames");
            return 0;
        }

        fclose(test);
        remove("dijkstra_frames/.write_test");
    }

    /* Remove old frames so a previous longer animation
       cannot contaminate the new GIF. */
    if (system("rm -f dijkstra_frames/frame_*.png "
               "dijkstra_frames/frame_*.dot") != 0)
    {
        printf("Could not clear old Dijkstra frames.\n");
        return 0;
    }

    int *distance = malloc((size_t)g->maxNodes * sizeof(int));
    int *parent = malloc((size_t)g->maxNodes * sizeof(int));
    int *visited = calloc((size_t)g->maxNodes, sizeof(int));
    int *finalPath = calloc((size_t)g->maxNodes, sizeof(int));

    if (distance == NULL || parent == NULL ||
        visited == NULL || finalPath == NULL)
    {
        free(distance);
        free(parent);
        free(visited);
        free(finalPath);
        printf("Memory allocation failed.\n");
        return 0;
    }

    for (int i = 0; i < g->maxNodes; i++)
    {
        distance[i] = INT_MAX;
        parent[i] = -1;
    }

    distance[source] = 0;

    int step = 0;
    int ok = renderDijkstraFrame(
        g, distance, visited, parent, NULL,
        source, source, target, step++
    );

    for (int count = 0; ok && count < g->maxNodes; count++)
    {
        int u = -1;
        int minDistance = INT_MAX;

        for (int i = 0; i < g->maxNodes; i++)
        {
            if (!visited[i] && distance[i] < minDistance)
            {
                minDistance = distance[i];
                u = i;
            }
        }

        if (u == -1)
            break;

        visited[u] = 1;

        if (!renderDijkstraFrame(
                g, distance, visited, parent, NULL,
                u, source, target, step++))
        {
            ok = 0;
            break;
        }

        if (u == target)
            break;

        EdgeNode *edge = getNeighbors(g, u);

        while (edge != NULL)
        {
            int v = edge->destinationId;
            int weight = edge->weight;

            if (v < 0 || v >= g->maxNodes ||
                !g->nodeExists[v] || weight < 0)
            {
                edge = edge->next;
                continue;
            }

            if (!visited[v] &&
                distance[u] != INT_MAX &&
                distance[u] <= INT_MAX - weight &&
                distance[u] + weight < distance[v])
            {
                distance[v] = distance[u] + weight;
                parent[v] = u;

                if (!renderDijkstraFrame(
                        g, distance, visited, parent, NULL,
                        u, source, target, step++))
                {
                    ok = 0;
                    break;
                }
            }

            edge = edge->next;
        }
    }

    if (ok && distance[target] != INT_MAX)
    {
        int node = target;
        int pathLength = 0;

        while (node != -1 && pathLength < g->maxNodes)
        {
            finalPath[node] = 1;
            pathLength++;

            if (node == source)
                break;

            node = parent[node];
        }

        if (node != source)
        {
            printf("Could not reconstruct the shortest path.\n");
            ok = 0;
        }
    }

    if (ok && !renderDijkstraFrame(
            g, distance, visited, parent,
            distance[target] == INT_MAX ? NULL : finalPath,
            -1, source, target, step++))
    {
        ok = 0;
    }

    if (ok && distance[target] == INT_MAX)
        printf("No path exists between %d and %d.\n",
               source, target);

    if (ok && distance[target] != INT_MAX)
        printf("Shortest distance: %d\n", distance[target]);

    free(distance);
    free(parent);
    free(visited);
    free(finalPath);

    if (!ok)
    {
        printf("Failed to generate Dijkstra frames.\n");
        return 0;
    }

    char command[512];

    int written = snprintf(
        command, sizeof(command),
        "ffmpeg -y -loglevel error -framerate 1 "
        "-i dijkstra_frames/frame_%%04d.png "
        "-vf \"scale=900:-1:flags=lanczos,"
        "split[s0][s1];[s0]palettegen[p];"
        "[s1][p]paletteuse\" "
        "-loop 0 dijkstra_animation.gif"
    );

    if (written < 0 || (size_t)written >= sizeof(command))
        return 0;

    if (system(command) != 0)
    {
        printf("FFmpeg failed to create the GIF.\n");
        return 0;
    }

    printf("Dijkstra animation created: dijkstra_animation.gif\n");
    printf("Frames saved in dijkstra_frames/.\n");

    return 1;
}
