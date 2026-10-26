#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "graph.h"

int exportGraphviz(const Graph *g, const char *dotFile,
                   const char *pngFile);

#endif
