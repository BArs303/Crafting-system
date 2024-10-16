#ifndef LOAD_H
#define LOAD_H
#include "item.h"

Set *load_items(const char *fname);
void load_recipes(Set *items, const char *fname);
#endif
