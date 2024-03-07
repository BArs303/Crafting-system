#include "advanced_types.h"

#ifndef STACK_h
#define STACK_h
typedef struct stack Stack;

struct stack
{
	void **top;
	void **base;
	int size;
	int capacity;
};

Stack* init_stack();
bool push(Stack *a, void *element);
void* pop(Stack *a);
void free_stack(Stack *a);
bool stack_is_empty(Stack *a);

#endif