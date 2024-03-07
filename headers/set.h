#include "advanced_types.h"

#ifndef SET_h
#define SET_h

typedef struct dynamic_array Set;



Set* init_set();
Set* init_set_with_length(int length);
void destruct_set(Set *a, void (*free_element)(void *element));
void print_set(Set *a, void (*print_element)(void *element));

void set_clear(Set *a, void (*free_element)(void *element));
void set_append(Set *dst, void *element);
void set_remove(Set *a, int index, void (*free_element)(void *element));

void* set_at_pos(Set *a, int index);

#endif