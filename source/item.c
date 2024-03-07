#include "item.h"

static Recipe* create_empty_recipe();
static Rds* create_empty_rds();

static void link_recipe(Item *a, Recipe *r);
static bool unlink_recipe(Item *a, Recipe *r);

static void print_rds(void *element);
static void inner_print_recipe(void *a);
static void destruct_quantity(void *element);
static void passive_destruct(void *element);
static void destruct_recipes(Darray *recipes);

static int find_index_by_id(Set *products, int id);
static int get_recipe_index(Item *a, Recipe *r);

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

void destruct_item(Item *a)
{
	destruct_darray(a->recipes, &passive_destruct);
	destruct_darray(a->used_in, &passive_destruct);
	free(a->name);
	free(a);
	return;
}

void add_recipe(Item *a)
{
	/*by default every recipe created with output of 1 product*/
	add_recipe_with_quantity(a, DEFAULT_PRODUCT_QUANTITY);
	return;
}

void add_recipe_with_quantity(Item *a, int product_quantity)
{
	add_recipe_with_opts(a, product_quantity, 1);
	return;
}

void add_recipe_with_opts(Item *a, int product_quantity, int num_of_components)
{
	Recipe *new_recipe;
	new_recipe = create_empty_recipe(num_of_components);
	add_product(new_recipe, a, product_quantity);
	return;
}

static Recipe* create_empty_recipe(int rds_size)
{
	Recipe *newr;
	newr = malloc(sizeof(Recipe));
	//need allocation check
	newr->products = create_empty_rds(1);
	newr->components = create_empty_rds(rds_size);
	return newr;
}

static Rds* create_empty_rds(int size)
{
	Rds *newRds;
	newRds = malloc(sizeof(Rds));
	newRds->items = init_set_with_length(size);
	newRds->quantity = init_darray_with_length(size);
	return newRds;
}

void destruct_rds(Rds *a)
{
	destruct_set(a->items, &passive_destruct);
	destruct_darray(a->quantity, &destruct_quantity);
	free(a);
	return;
}

void destruct_recipe(Recipe *a)
{
	Item *product;
	for(int i = 0; i < a->products->items->size; i++)
	{
		product = darray_at_pos(a->products->items, i);
		unlink_recipe(product, a);
	}
	destruct_rds(a->products);
	destruct_rds(a->components);
	free(a);
}

void add_product(Recipe *dest, Item *src, int product_quantity)
{
	int *pq;
	pq = malloc(sizeof(int));
	//need allocation check
	*pq = product_quantity;
	set_append(dest->products->items, src);
	darray_append(dest->products->quantity, pq);
	link_recipe(src, dest);
	return;
}

static void link_recipe(Item *a, Recipe *new_recipe)
{
	darray_append(a->recipes, new_recipe);
	return;
}

static bool unlink_recipe(Item *a, Recipe *r)
{
	int recipe_index = get_recipe_index(a, r);
	if(recipe_index >= 0)
	{
		darray_remove(a->recipes, recipe_index, &passive_destruct);
		return true;
	}
	else
	{
		printf("data integrity is compromised\n");
		print_item(a);
		exit(-1);
	}
}

static int get_recipe_index(Item *a, Recipe *r)
{
	Recipe *item_recipe;
	for(int i = 0; i < a->recipes->size; i++)
	{
		item_recipe = darray_at_pos(a->recipes, i);
		if(item_recipe == r)
		{
			return i;
		}
	}
	return -1;
}

void add_component(Recipe *dest, Item *src, int component_quantity)
{
	int *cq;
	cq = malloc(sizeof(int));
	//need allocation check
	*cq = component_quantity;
	set_append(dest->components->items, src);
	darray_append(dest->components->quantity, cq);
	set_append(src->used_in, dest);
	return;
}

void destruct_item_and_recipes(Item *a)//not working function
{
	while(a->recipes->size > 0)
	{
		destruct_recipe(darray_at_pos(a->recipes, 0));
	}
	destruct_item(a);
}

static void destruct_quantity(void *element)
{
	free(element);
	return;
}
static void passive_destruct(void *element)
{
	return;
}

void remove_product_by_id(Recipe *a, int id)
{
	int index = find_index_by_id(a->products->items, id);
	if(index >= 0)
	{
		remove_product_by_index(a, index);
	}
	else
	{
		printf("No product with such id\n");
	}
	return;
}

void remove_product_by_index(Recipe *a, int index)
{
	Item *item = set_at_pos(a->products->items, index);
	if(item)
	{
		unlink_recipe(item, a);
		set_remove(a->products->items, index, &passive_destruct);
		darray_remove(a->products->quantity, index, &destruct_quantity);
	}
	else
	{
		printf("No product with such index\n");
	}
	return;
}

static int find_index_by_id(Set *products, int id)
{
	Item *tmp;
	for(int i = 0; i < products->size; i++)
	{
		tmp = set_at_pos(products, i);
		if(tmp->id == id)
			return i;
	}
	return -1;
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
	print_rds(tmp->products);
	printf("Components:\n");
	print_rds(tmp->components);
}

Recipe* get_recipe(Item *a, int index)
{
	return darray_at_pos(a->recipes, index);
}
static void print_rds(void *element)
{
	Item *tmp;
	Rds *a;
	int *q;
	a = element;
	for(int i = 0; i < a->items->size; i++)
	{
		tmp = set_at_pos(a->items, i);
		q =darray_at_pos(a->quantity, i);
		printf("%s: %d\n", tmp->name, *q);
	}
	printf("\n");
	return;
}