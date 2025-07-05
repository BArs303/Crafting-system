#include "item.h"

static void print_rcomponent(void *element, void *params);
static void inner_print_recipe(void *a, void *params);

static IData* create_idata(char *name, float price);
static void delete_idata(IData *data);

static RData* create_rdata();
static void delete_rdata(RData *data);

Item *create_item(int id, char *name)
{
	Item *ret;
	ret = malloc(sizeof(Item));
	ret->id = id;
	/*need allocation check*/

	ret->data = create_idata(name, 0);
	ret->recipes = init_darray();
	ret->used_in = init_darray();
	return ret;
}

static IData* create_idata(char *name, float price)
{
	IData *ret;
	ret = malloc(sizeof(IData));

	ret->name = name;
	ret->price = price;
	return ret;
}

static void delete_idata(IData *data)
{
	if(data)
	{
		free(data->name);
		free(data);
	}
}

void delete_item(void *i, void *params)
{
	Item* item;
	item = i;
	if(item)
	{
		delete_idata(item->data);
		free(item);
	}
}

Recipe *create_recipe()
{
	Recipe *newr;
	newr = malloc(sizeof(Recipe));
	newr->data = create_rdata();
	//need allocation check

	newr->products = NULL;
	newr->components = NULL;
	return newr;
}

static RData* create_rdata()
{
	RData *ret;
	ret = malloc(sizeof(RData));

	return ret;
}

static void delete_rdata(RData *data)
{
	free(data);
}

void delete_recipe(void *r)
{
	Recipe *recipe;
	recipe = r;
	delete_rdata(recipe->data);
	if(recipe->products)
		delete_darray(recipe->products, &delete_rcomponent, NULL);

	if(recipe->components)
		delete_darray(recipe->components, &delete_rcomponent, NULL);
	free(recipe);
}

Rcomponent* create_rcomponent(Item *item, int q)
{
	Rcomponent *ret;
	ret = malloc(sizeof(Rcomponent));

	ret->item = item;
	ret->quantity = q;
	return ret;
}

void delete_rcomponent(void *rcomponent, void *params)
{
	free(rcomponent);
}

void link_recipe(Recipe *recipe)
{
	Rcomponent *tmp;
	for(int i = 0; i < recipe->products->size; i++)
	{
		tmp = darray_at_pos(recipe->products, i);
		if(tmp->item)
		{
			darray_append(tmp->item->recipes, recipe);
		}
	}
	for(int i = 0; i < recipe->components->size; i++)
	{
		tmp = darray_at_pos(recipe->components, i);
		if(tmp->item)
		{
			darray_append(tmp->item->used_in, recipe);
		}
	}
	return;
}

int rcomponent_comparison(void *arg1, void *arg2)
{
	return id_comparison
	(
		CAST(Rcomponent*, arg1)->item, 
		CAST(Rcomponent*, arg2)->item 
	);
}

int id_comparison(void *arg1, void *arg2)
{
	return (((Item*)arg1)->id) - (((Item*)arg2)->id);
}

void print_item(void *item)
{
	Item *a;
	a = item;
	printf("Items name %s\n"
		"Item id: %d\nUsed in %d recipes\n", a->data->name, a->id, a->used_in->size);
	//print_darray(a->used_in, &inner_print_recipe);//type change
	printf("Can be crafted:\n");
	print_darray(a->recipes, &inner_print_recipe, NULL);
	printf("----end of item----\n");
}

void print_recipe(Recipe *a)
{
	inner_print_recipe(a, NULL);
	return;
}

static void inner_print_recipe(void *a, void *params)
{
	Recipe *tmp = a;
	printf("Products:\n");
	print_darray(tmp->products, &print_rcomponent, NULL);//type change
	printf("\nComponents:\n");
	print_darray(tmp->components, &print_rcomponent, NULL);//type change
	printf("\n");
}

static void print_rcomponent(void *element, void *params)
{
	Rcomponent *tmp = element;
	if(tmp)
	{
        if(tmp->item)
        {
            printf("%d %s: %d\n", tmp->item->id, tmp->item->data->name, tmp->quantity);
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
}

Darray* get_components(Item *a, unsigned int recipe_number)
{
	Recipe *c;
	c = darray_at_pos(a->recipes, recipe_number);

	return c ? c->components : NULL;
}

Darray* get_products(Item *a, unsigned int recipe_number)
{
	Recipe *c;
	c = darray_at_pos(a->recipes, recipe_number);
	return c ? c->products : NULL;
}

/*IO Section*/

Item* convert_json_to_item(HNode *a)
{
	Item *item;
	JSON *value;
	int id;
	item = NULL;
	if(a && a->value)
	{
		id = atoi(a->key);
		value = a->value;
		if(value->type == type_string)
		{
			item = create_item(id, value->value.string);
		}
	}
	return item;
}

JSON* convert_item_to_json(Item *item)
{
	JSON *a = NULL;
	if(item)
	{
		a = malloc(sizeof(JSON));
		a->value.string = item->data->name;
		a->type = type_string;
	}
	return a;
}

Rcomponent* convert_json_to_rc(Set *items, JSON *a)
{
	JSON *json_id, *json_quantity;
	Item *ptr;
	Item f; //used in find function
	if(a && a->type == type_object)
	{
		json_id = hmap_get(a->value.object, "id");
		json_quantity = hmap_get(a->value.object, "quantity");
		if(json_id && json_quantity)
		{
			f.id = json_id->value.number;
			ptr = set_find(items, &f, &id_comparison);
			if(ptr)
			{
				return create_rcomponent(ptr, json_quantity->value.number);
			}
			else
			{
				fprintf(stderr, "Rcomponent convert error: item with id %d doesn't exists\n", f.id);
			}
		}
		else
		{
			perror("Rcomponent convert error: rcomponent field name error\n");
		}
	}
	return NULL;
}

Darray* convert_json_to_rca(Set *items, JSON *a) //rca - recipe component array
{
	List *materials_list;
	Rcomponent *unit;
	Darray *ret;
	JSON *json_rcomponent;

	materials_list = a->value.list;// a is not null. Check in function json_to_recipe
	ret = init_darray_with_length(materials_list->size);
	for(int i = 0; i < materials_list->size; i++)
	{
		json_rcomponent = list_at_pos(materials_list, i);
		unit = convert_json_to_rc(items, json_rcomponent);
		if(unit)
		{
			darray_insert(ret, unit, ret->size);//replace to List maybe??
		}
	}
	return ret;
}
Recipe* convert_json_to_recipe(Set *items, JSON *a)
{
	Recipe *ret;
	JSON *json_products, *json_components;
	if(a && a->type == type_object)
	{
		json_components = hmap_get(a->value.object, "components");
		json_products = hmap_get(a->value.object, "products");
		if(!json_products || !json_components)
		{
			perror("Recipe convert error: missing field\n");
			return NULL;
		}

		ret = create_recipe();
		ret->products = convert_json_to_rca(items, json_products);
		ret->components = convert_json_to_rca(items, json_components);
		if(ret->products->size == 0 || ret->components->size == 0)
		{
			delete_recipe(ret);
			return NULL;
		}
		return ret;
	}
	return NULL;//ret??
}

/*static void debug_json_errors(JSON *a)
{
	printf("%s\n", json_stringify(a));
}*/
/*JSON* unload_recipes(Darray *recipes)
{
	return NULL;
}*/
