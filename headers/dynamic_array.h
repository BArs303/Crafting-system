#include "advanced_types.h"

#ifndef DYNAMIC_ARRAY_h
#define DYNAMIC_ARRAY_h

typedef struct dynamic_array Darray;

struct dynamic_array
{
	void **array;
	int size;
	int capacity;
};


Darray* init_darray();
Darray* init_darray_with_length(int length);
void destruct_darray(Darray *a, void (*free_element)(void *element));
void print_darray(Darray *a, void (*print_element)(void *element));

void darray_clear(Darray *a, void (*free_element)(void *element));
void darray_append(Darray *dst, void *element);
void darray_insert(Darray *dst, void *element, int index);
void darray_remove(Darray *a, int index, void (*free_element)(void *element));

void* darray_at_pos(Darray *a, int index);
void* darray_replace(Darray *a, int index, void *new_element, void(*free_element)(void *element));

#endif