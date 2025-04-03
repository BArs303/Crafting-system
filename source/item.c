#include "item.h"

static void print_rcomponent(void *element);
static void inner_print_recipe(void *a);

static int rcomponent_sort(void *arg1, void *arg2);
static void delete_rcomponent(void *rcomponent);

Item *create_item(int id, char *name)
{
	Item *newi;
	newi = malloc(sizeof(Item));
	newi->id = id;
	/*need allocation check*/

	newi->data = malloc(sizeof(IData));
	newi->data->name = name;
	newi->data->price = 0;

	newi->recipes = init_darray();
	newi->used_in = init_darray();
	return newi;
}

Recipe *create_recipe()
{
	Recipe *newr;
	newr = malloc(sizeof(Recipe));
	//need allocation check
	newr->products = NULL;
	newr->components = NULL;
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

void link_recipe(Recipe *a)
{
	Rcomponent *tmp;
	for(int i = 0; i < a->products->size; i++)
	{
		tmp = darray_at_pos(a->products, i);
		if(tmp->item)
		{
			darray_append(tmp->item->recipes, a);
		}
	}
	for(int i = 0; i < a->components->size; i++)
	{
		tmp = darray_at_pos(a->components, i);
		if(tmp->item)
		{
			darray_append(tmp->item->used_in, a);
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

void delete_item(void *item)
{
	Item *a;
	Recipe *recipe;
	Darray *products;
	int i;
	a = item;
	//if no recipe and never used
	if(a)
	{
		//used in list doesn't fully implemented
		delete_darray(a->used_in, passive_destruct);

		/*if you want to delete an item and 
		its recipe contains only one product,
		the recipe must be delted
		if the recipe contains other products,
		the deleted item must be removed from the products list*/
		for(i = 0; i < a->recipes->size; i++)
		{
			recipe = get_recipe(a, i);
			products = recipe->products;
			if(products->size < 2) //recipe needed only for this item or doesn't have products
			{
				delete_recipe(recipe);
			}
		}
		delete_darray(a->recipes, passive_destruct);
		free(a->data->name);
		free(a->data);

		free(a);
	}
	return;
}

void delete_recipe(void *recipe)
{
	Recipe *a;
	a = recipe;
	if(a->products)
		delete_darray(a->products, &delete_rcomponent);

	if(a->components)
		delete_darray(a->components, &delete_rcomponent);
	free(a);
}

static void delete_rcomponent(void *rcomponent)
{
	free(rcomponent);
}

void print_item(void *item)
{
	Item *a;
	a = item;
	printf("Items name %s\n"
		"Item id: %d\nUsed in %d recipes\n", a->data->name, a->id, a->used_in->size);
	//print_darray(a->used_in, &inner_print_recipe);//type change
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
	print_darray(tmp->products, &print_rcomponent);//type change
	printf("\nComponents:\n");
	print_darray(tmp->components, &print_rcomponent);//type change
	printf("\n");
}

Recipe* get_recipe(Item *a, int index)
{
	return darray_at_pos(a->recipes, index);
}

Darray* get_components(Item *a, int index)
{
    Recipe *tmp = get_recipe(a, index);
    if(tmp)
    {
        return tmp->components;
    }
    return NULL;
}

Darray* get_products(Item *a, int index)
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

	return;
}

/*IO Section*/

Item* convert_json_to_item(JSON *a)
{
	Item *item;
	int id;
	item = NULL;
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
		a->value.string = item->data->name;
		a->type = type_String;
	}
	return a;
}

Rcomponent* convert_json_to_rc(Set *items, JSON *a)
{
	JSON *json_id, *json_quantity;
	Item *ptr;
	Item f; //used in find function
	if(a && a->type == type_Object)
	{
		json_id = list_at_pos(a->value.object, 1); //replace to hashtable
		json_quantity = list_at_pos(a->value.object, 0);
		if(strcmp(json_id->key, "id") == 0 && strcmp(json_quantity->key, "quantity") == 0)//useless with hashtable
		{
			f.id = json_id->value.number;
			ptr = set_find(items, &f, &id_comparison);
			if(ptr)
			{
				return create_rcomponent(ptr, json_quantity->value.number);
			}
			else
			{
				printf("Rcomponent convert error: item with such id does't exists\n");
			}
		}
		else
		{
			printf("Rcomponent convert error: rcomponent field name error\n");
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
			darray_append(ret, unit);//replace to List maybe??
		}
	}
	return ret;
}
Recipe* convert_json_to_recipe(Set *items, JSON *a)
{
	Recipe *ret;
	JSON *json_products, *json_components;
	if(a && a->type == type_Object)
	{
		json_components = list_at_pos(a->value.object, 0);//hashtable
		json_products = list_at_pos(a->value.object, 1);
		if(!json_products || !json_components)
		{
			printf("Recipe convert error: missing field\n");
			return NULL;
		}
		if(strcmp(json_products->key, "products") == 0 && strcmp(json_components->key, "components") == 0)//hashtable required
		{
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
		else
		{
			printf("Recipe convert error: Invalid field name\n");
		}
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
