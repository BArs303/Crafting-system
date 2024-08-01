#include "item.h"


static void print_rcomponent(void *element);
static void inner_print_recipe(void *a);

static void passive_destruct(void *element);

static int rcomponent_sort(void *arg1, void *arg2);
static int id_comparison(void *arg1, void *arg2);


Item *create_item(int id, char *name)
{
	Item *newi;
	newi = malloc(sizeof(Item));
	//need allocation check
	newi->id = id;
	newi->name = name;
	newi->recipes = init_darray();
	newi->used_in = init_darray();
	return newi;
}

Recipe *create_recipe()
{
	Recipe *newr;
	newr = malloc(sizeof(Recipe));
	//need allocation check
	newr->products = init_set();
	newr->components = init_set();
	return newr;
}

Rcomponent* create_rcomponent(Item *a, int q)
{
	Rcomponent *ret;
	ret = malloc(sizeof(Rcomponent));
	ret->item = a;
	ret->quantity = q;
	return ret;
}

bool add_component(Recipe *dest, Rcomponent *element)
{
	return rcomponent_set_insert(dest->components, element);
}

bool add_product(Recipe *dest, Rcomponent *element)
{
	return rcomponent_set_insert(dest->products, element);
}

void link_recipe(Recipe *a)
{
	Rcomponent *tmp;
	for(int i = 0; i < a->products->size; i++)
	{
		tmp = set_at_pos(a->products, i);
		if(tmp->item)
		{
			darray_append(tmp->item->recipes, a);
		}
	}
	return;
}

bool rcomponent_set_insert(Set *a, Rcomponent *element)
{
	return s_set_insert(a, element, &rcomponent_sort);
}

static int rcomponent_sort(void *arg1, void *arg2)
{
	Rcomponent *a, *b;
	a = arg1;
	b = arg2;
	return a->item->id - b->item->id;
}

Rcomponent* rcomponent_find(Set *src, Rcomponent *element)
{
	return set_binary_search(src, element, &rcomponent_sort);
}

int Rcomponent_find_index(Set *src, Rcomponent *element)
{
	return set_search_index(src, element, &rcomponent_sort);
}

bool item_set_insert(Set *a, Item *element)
{
	return s_set_insert(a, element, &id_comparison);
}

static int id_comparison(void *arg1, void *arg2)
{
	return (((Item*)arg1)->id) - (((Item*)arg2)->id);
}

void destruct_item(Item *a)
{
	destruct_darray(a->recipes, &passive_destruct);
	destruct_darray(a->used_in, &passive_destruct);
	free(a->name);
	free(a);
	return;
}
static void passive_destruct(void *element)
{
	return;
}

void print_item(Item *a)
{
	printf("Items name %s\n"
		"Item id: %d\nUsed in:\n", a->name, a->id);
	print_set(a->used_in, &inner_print_recipe);
	printf("Can be crafted:\n");
	print_darray(a->recipes, &inner_print_recipe);
	printf("----end of item----\n");
}
void print_recipe(Recipe *a)
{
	inner_print_recipe(a);
	return;
}

static void inner_print_recipe(void *a)
{
	Recipe *tmp = a;
	printf("Products:\n");
	print_set(tmp->products, &print_rcomponent);
	printf("\nComponents:\n");
	print_set(tmp->components, &print_rcomponent);
	printf("\n");
}

Recipe* get_recipe(Item *a, int index)
{
	return darray_at_pos(a->recipes, index);
}

Set* get_components(Item *a, int index)
{
    Recipe *tmp = get_recipe(a, index);
    if(tmp)
    {
        return tmp->components;
    }
    return NULL;
}

Set* get_products(Item *a, int index)
{
    Recipe *tmp = get_recipe(a, index);
    if(tmp)
    {
        return tmp->products;
    }
    return NULL;
}
static void print_rcomponent(void *element)
{
	Rcomponent *tmp = element;
	if(tmp)
	{
        if(tmp->item)
        {
            printf("%s: %d\n", tmp->item->name, tmp->quantity);
        }
        else
        {
            printf("\nNULL ITEM\n");
        }
	}
	else
	{
        printf("Error: Rcomponents is equal 0");
	}

	return;
}

/*IO Section*/

Item* convert_json_to_item(JSON *a)
{
	Item *item = NULL;
	int id;
	if(a->type == type_String)
	{
		id = str_to_int(a->key);
		item = create_item(id, a->value.string);
	}
	return item;
}

JSON* convert_item_to_json(Item *item)
{
	JSON *a = NULL;
	if(item)
	{
		a = malloc(sizeof(JSON));
		a->key = int_to_str(item->id);
		a->value.string = item->name;
		a->type = type_String;
	}
	return a;
}

/*JSON* unload_recipes(Darray *recipes)
{
	return NULL;
}*/
