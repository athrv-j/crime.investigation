
#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include "case.h"

int saveCase(const char *filename, Case *c);
int loadCase(const char *filename, Case *c);

#endif
