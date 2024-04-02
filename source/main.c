#include "item.h"
#include "json.h"

Darray* items_creation();
Item* find_item_by_id(Darray *items, int id);
Item* item_binary_search(Darray *items, int id, int start, int end);
void load_recipes(Darray *items);

int main()
{
	Darray *items;
	Item *a;
	items = items_creation();
	/*for(int i = 0; i < items->size; i++)
	{
		print_item(darray_at_pos(items, i));
	}*/
	printf("RECIPES\n");
	load_recipes(items);
	//need to free array of items
	return 0;
}

Darray* items_creation()
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
	JSON *inp, *json_recipes,  *json_recipe, *json_products, *json_components, *json_rcomponent, *json_id, *json_quantity;
	List *jrecipes, *jproducts, *jcomponents;
	inp = json_parse_file("recipes.txt");
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
				json_products = list_at_pos(json_recipe->value.object, 0);
				json_components = list_at_pos(json_recipe->value.object, 1);
				if(strcmp(json_products->key, "products") == 0 && strcmp(json_components->key, "components") == 0)
				{
					jproducts = json_products->value.list;
					jcomponents = json_components->value.list;
					printf("products\n");
					for(int j = 0; j < jproducts->size; j++)
					{
						json_rcomponent = list_at_pos(jproducts, j);
						if(json_rcomponent->type == type_Object)
						{
							json_id = list_at_pos(json_rcomponent->value.object, 0);
							json_quantity = list_at_pos(json_rcomponent->value.object, 1);
							if(strcmp(json_id->key, "id") == 0 && strcmp(json_quantity->key, "quantity") == 0)
							{
								printf("id: %d\nquantity: %d\n", json_id->value.number, json_quantity->value.number);
							}
							else
							{
								printf("rcomponent name error\n");
							}
						}
					}
					printf("components\n");
					for(int j = 0; j < jcomponents->size; j++)
					{
						json_rcomponent = list_at_pos(jcomponents, j);
						if(json_rcomponent->type == type_Object)
						{
							json_id = list_at_pos(json_rcomponent->value.object, 0);
							json_quantity = list_at_pos(json_rcomponent->value.object, 1);
							if(strcmp(json_id->key, "id") == 0 && strcmp(json_quantity->key, "quantity") == 0)
							{
								printf("id: %d\nquantity: %d\n", json_id->value.number, json_quantity->value.number);
							}
							else
							{
								printf("rcomponent name error\n");
							}
						}
					}
				}
				else
				{
					printf("name error\n");
				}
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
