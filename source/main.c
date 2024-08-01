#include "advanced_types.h"
#include "item.h"

Darray* items_creation();
Item* find_item_by_id(Darray *items, int id);
Item* item_binary_search(Darray *items, int id, int start, int end);
void load_recipes(Darray *items);

Recipe* form_recipe(Set *items, JSON *products, JSON *components);
void hfunc(Set *items,Set *dst,  JSON *materials);
int div_up(int x, int y);
bool check_item_existence(Set *summary, void *element);

void show_tree(Item *root);
Rcomponent* form_scomponent(Item *a, int quantity);
int calculate_surplus(Item *a, int quantity);

void debugSummaryPrint(Set *summary);
void debugQueuePrint(Darray *queue, int index);
void debugSurplusPrint(Set *surplus);

void update_surplus(Darray *queue, Set *surplus, Rcomponent *component, int quantity);
void update_summary(Set *summary, Rcomponent *required, Rcomponent *product);
void substract_surplus(Set *summary, Set *surplus);

int main()
{
	Darray *items;

	items = items_creation();
	printf("RECIPES\n");
	load_recipes(items);
	printf("Loaded\n");

	show_tree(find_item_by_id(items, 28710));
	printf("\n");

	//need to free array of items
	return 0;
}

bool check_item_existance(Set *summary, void *element)
{
	for(int i = 0; i < summary->size; i++)
	{
		if(set_at_pos(summary, i) == element)
			return false;
	}
	return true;
}

void show_tree(Item *root)
{
    Darray *queue;
    Set *summary, *surplus;
    Rcomponent *component, *product, *required;
	Rcomponent *addable, *surplus_component, *csummary;
	Set *components, *products;
	int quantity;

    queue = init_darray();
    summary = init_set();
    surplus = init_set();
	component = create_rcomponent(root, 1);
	darray_append(queue, component);

    for(int i = 0; i < queue->size; i++)
    {
        required = darray_at_pos(queue, i);
        components = get_components(required->item, 0);
        products = get_products(required->item, 0);
        if(components && products)
        {
            product = set_at_pos(products, 0);
			printf("product %s\n", product->item->name);
            for(int j = 0; j < components->size; j++)
            {
                component = set_at_pos(components, j);
				printf("component %s %d\n", component->item->name, component->item->id);
				quantity = component->quantity * div_up(required->quantity, product->quantity);
				surplus_component = rcomponent_find(surplus, component);//??
				update_surplus(queue, surplus, component, quantity);

				/*printf("Queue\n");
				debugQueuePrint(queue, i);
				printf("Surplus\n");
				debugSurplusPrint(surplus);*/

            }
			printf("\n");
			update_summary(summary, required, product);
        }
		else
		{
			update_summary(summary, required, NULL);
		}
    }
	substract_surplus(summary, surplus);

	printf("Queue\n");
	debugQueuePrint(queue, 0);
	printf("Summary\n");
	debugSummaryPrint(summary);
	printf("Surplus\n");
	debugSurplusPrint(surplus);
}

void substract_surplus(Set *summary, Set *surplus)
{
	Rcomponent *unit, *sunit;
	for(int i = 0; i < summary->size; i++)
	{
		unit = set_at_pos(summary, i);
		sunit = rcomponent_find(surplus, unit);
		if(sunit)
		{
			unit->quantity -= sunit->quantity;
		}
	}
	return;
}

void update_summary(Set *summary, Rcomponent *required, Rcomponent *product)
{
	Rcomponent *csummary;
	int quantity;
	if(product)
	{
		quantity = product->quantity * div_up(required->quantity, product->quantity);
	}
	else
	{
		quantity = required->quantity;
	}

	csummary = rcomponent_find(summary, required);
	if(csummary)
	{
		csummary->quantity += quantity;
	}
	else
	{
		rcomponent_set_insert(summary, create_rcomponent(required->item, quantity));
	}
	return;
}

void update_surplus(Darray *queue, Set *surplus, Rcomponent *component, int quantity)
{
	Rcomponent *surplus_component, *addable;
	int s;

	surplus_component = rcomponent_find(surplus, component);
	s = calculate_surplus(component->item, quantity);
	//printf("%s need %d ", component->item->name, quantity);

	if(surplus_component)
	{
		//printf("already exists with quantity %d ", surplus_component->quantity);
		if(surplus_component->quantity > quantity)
		{
			//printf("do not add\n");
			surplus_component->quantity -= quantity;
		}
		else
		{
			quantity -= surplus_component->quantity;
			surplus_component->quantity = calculate_surplus(component->item, quantity);
			//printf("add to queue with %d\n", quantity);
			addable = create_rcomponent(component->item, quantity);
			darray_append(queue, addable);
		}
	}
	else
	{
		//printf("just adding\n");
		addable = create_rcomponent(component->item, quantity);
		if(s != 0)
		{
			rcomponent_set_insert(surplus, create_rcomponent(component->item, s));
		}
		darray_append(queue, addable);
	}
	return;
}
void debugSummaryPrint(Set *summary)
{
	Rcomponent *a;
	for(int i = 0; i < summary->size; i++)
	{
		a = darray_at_pos(summary, i);
		if(a->item->recipes->size == 0)
		{
			printf("%s %d\n", a->item->name, a->quantity);
		}
	}
	printf("\n");
}
void debugSurplusPrint(Set *surplus)
{
	Rcomponent *a;
	for(int i = 0; i < surplus->size; i++)
	{
		a = darray_at_pos(surplus, i);
		printf("%s %d\n", a->item->name, a->quantity);
	}
	printf("\n");
}
void debugQueuePrint(Darray *queue, int index)
{
	Rcomponent *a;
	for(int i = 0; i < queue->size; i++)
	{
		a = darray_at_pos(queue, i);
		if(i == index)
		{
			printf("id: %d %s %d<-----\n", a->item->id, a->item->name, a->quantity);
		}
		else
		{
			printf("id: %d %s %d\n", a->item->id, a->item->name, a->quantity);
		}
	}
	printf("\n");

}

int calculate_surplus(Item *a, int quantity)
{
	Set *products;
	Rcomponent *product;
	int result = 0;
	products = get_products(a, 0);
	if(products)
	{
		product = set_at_pos(products, 0);
		if(quantity % product->quantity != 0)
		{
			result = product->quantity - quantity % product->quantity;
		}
	}
	return result;
}
int div_up(int x, int y)
{
    return (x-1)/y+1;
}


Set* items_creation()
{
	JSON *a, *field;
	List *object;
	Darray *items = NULL;
	a = json_parse_file("items.json");
	if(a)
	{
		object = a->value.object;
		items = init_darray_with_length(object->size);
		for(int i = 0; i < object->size; i++)
		{
			field = list_at_pos(object, i);
			darray_append(items, convert_json_to_item(field));
		}
	}
	return items;
}

void load_recipes(Darray *items)
{
	JSON *inp, *json_recipes,  *json_recipe, *json_products, *json_components;
	List *jrecipes;
	Recipe *unit;
	inp = json_parse_file("recipes.json");
	if(inp->type == type_Object)
	{
		/*json_Object equals List*/
		json_recipes = list_at_pos(inp->value.object, 0);
		jrecipes = json_recipes->value.list;
		for(int i = 0; i < jrecipes->size; i++)
		{
			json_recipe = list_at_pos(jrecipes, i);
			if(json_recipe->type == type_Object)
			{
				json_products = list_at_pos(json_recipe->value.object, 1);
				json_components = list_at_pos(json_recipe->value.object, 0);
				if(!json_products || !json_components)
				{
					printf("error\n");
					continue;
				}
				if(strcmp(json_products->key, "products") == 0 && strcmp(json_components->key, "components") == 0 )
				{
					unit = form_recipe(items, json_products, json_components);
					link_recipe(unit);
				}
				else
				{
					printf("name error\n");
				}
			}
		}
	}
}

Recipe* form_recipe(Set *items, JSON *products, JSON *components)
{
	Recipe *ret = create_recipe();
	hfunc(items, ret->products, products);
	hfunc(items, ret->components, components);
	return ret;
}

void hfunc(Set *items, Set *dst, JSON *materials)
{
    Rcomponent *unit;
	Item *find_result;
    JSON *json_rcomponent, *json_id, *json_quantity;
    List *materials_list;

    materials_list = materials->value.list;
    for(int j = 0; j < materials_list->size; j++)
    {
        json_rcomponent = list_at_pos(materials_list, j);
        if(json_rcomponent->type == type_Object)
        {
            json_id = list_at_pos(json_rcomponent->value.object, 1);
            json_quantity = list_at_pos(json_rcomponent->value.object, 0);
            if(strcmp(json_id->key, "id") == 0 && strcmp(json_quantity->key, "quantity") == 0)
            {
				find_result = find_item_by_id(items, json_id->value.number);
				if(find_result)
				{
					unit = create_rcomponent(find_result, json_quantity->value.number);
					set_insert(dst, unit);
				}
				else
				{
					continue;
				}
            }
            else
            {
                printf("rcomponent name error\n");
            }
        }
    }
}
Item* find_item_by_id(Darray *items, int id)
{
	return item_binary_search(items, id, 0, items->size-1);
}

Item* item_binary_search(Darray *items, int id, int start, int end)
{
	Item *mitem;
	int middle;
	while(start <= end)
	{
		middle = (start+end)/2;
		mitem = darray_at_pos(items, middle);
		if(mitem->id == id)
		{
			return mitem;
		}
		else if(mitem->id < id)
		{
			start = middle + 1;
		}
		else
		{
			end = middle - 1;
		}
	}
	return NULL;
}
