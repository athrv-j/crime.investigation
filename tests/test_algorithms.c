#include <stdio.h>
#include "../headers/graph.h"
#include "../headers/bfs.h"
#include "../headers/dfs.h"
#include "../headers/dijkstra.h"

int main() {
    // Create graph
    Graph *g = createGraph(6);
    
    // Add nodes
    for (int i = 0; i < 6; i++) {
        addNode(g, i);
    }
    
    // Add edges (unweighted for BFS/DFS, weighted for Dijkstra)
    addEdge(g, 0, 1, "CONTACT", 1);      // 0→1 distance 1
    addEdge(g, 0, 2, "VISITED", 1);      // 0→2 distance 1
    addEdge(g, 1, 3, "CONTACT", 1);      // 1→3 distance 1
    addEdge(g, 2, 3, "VISITED", 2);      // 2→3 distance 2
    addEdge(g, 3, 4, "HAS_EVIDENCE", 1); // 3→4 distance 1
    addEdge(g, 3, 5, "CONTACT", 3);      // 3→5 distance 3
    
    printf("===== ALGORITHM TEST =====\n\n");
    
    // Test BFS (shortest path unweighted)
    printf("--- BFS: Path from 0 to 4 ---\n");
    BFSResult *bfsResult = bfs(g, 0, 4);
    if (bfsResult->pathLength > 0) {
        printf("Path: ");
        for (int i = 0; i < bfsResult->pathLength; i++) {
            printf("%d ", bfsResult->path[i]);
        }
        printf("\nLength: %d\n\n", bfsResult->pathLength);
    } else {
        printf("No path found\n\n");
    }
    freeBFSResult(bfsResult);
    
    // Test DFS (explore all reachable)
    printf("--- DFS: All reachable from 0 ---\n");
    DFSResult *dfsResult = dfs(g, 0);
    printf("Reachable nodes: ");
    for (int i = 0; i < dfsResult->count; i++) {
        printf("%d ", dfsResult->reachable[i]);
    }
    printf("\nCount: %d\n\n", dfsResult->count);
    freeDFSResult(dfsResult);
    
    // Test Dijkstra (shortest path weighted)
    printf("--- Dijkstra: Shortest route from 0 to 5 ---\n");
    DijkstraResult *dijkResult = dijkstra(g, 0, 5);
    if (dijkResult->pathLength > 0) {
        printf("Path: ");
        for (int i = 0; i < dijkResult->pathLength; i++) {
            printf("%d ", dijkResult->path[i]);
        }
        printf("\nDistance: %d\n\n", dijkResult->distance);
    } else {
        printf("No path found\n\n");
    }
    freeDijkstraResult(dijkResult);
    
    freeGraph(g);
    return 0;
}