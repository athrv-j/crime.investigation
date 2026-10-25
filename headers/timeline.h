
#ifndef TIMELINE_H
#define TIMELINE_H

#include "struct.h"

int compareEvents(const void *a, const void *b);
void sortEvents(struct Event events[], int count);

#endif
