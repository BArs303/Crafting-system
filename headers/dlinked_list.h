#include "advanced_types.h"

#ifndef DLINKED_LIST_h
#define DLINKED_LIST_h
#define DEBUG

#ifdef DEBUG
#define SHOW_LIST(dlist) debug_print(dlist);
#else
#define SHOW
#endif

typedef struct node Node;
typedef struct dlist List;

struct node
{
	void *data;
	Node *next;
	Node *previous;
};

struct dlist
{
	Node *head;
	Node *tail;
	int size;

	Node *last_used;
	int last_index;
};

List* init_list();
void destruct_list(List *a, void (*free_element)(void *element));
void print_list(List *a, void (*print_element)(void *element));
void debug_print(List *a);

void list_append(List *dst, void *element);
void list_insert(List *dst, void *element, int index);

void* list_at_pos(List *a, int index);
void* list_remove(List *a, int index);

#endif