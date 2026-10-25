
#ifndef RANKING_H
#define RANKING_H

#include "graph.h"

int scoreLead(int edgeCount, int evidenceCount);
void rankLeads(Graph *g, int *evidenceCount, int n);

#endif
