#include "advanced_types.h"
#include "item.h"

Darray* items_creation();
Item* find_item_by_id(Darray *items, int id);
Item* item_binary_search(Darray *items, int id, int start, int end);
void load_recipes(Darray *items);
Set* hfunc(Darray *items, JSON *materials);
Rcomponent* create_rcomponent(Item *a, int q);
int div_up(int x, int y);

void show_tree(Item *root);
Rcomponent* form_scomponent(Item *a, int quantity);

bool check_rcomponent(Set *summary, void *element);
int rcomponent_sort(void *arg1, void *arg2);
bool check_item(Set *summary, void *element);
int main()
{
	Darray *items;

	items = items_creation();
	printf("RECIPES\n");
	load_recipes(items);
	/*for(int i = 0; i < items->size; i++)
	{
		a = darray_at_pos(items, i);
		if(a->recipes->size > 0)
		{
            print_item(a);
		}
	}*/
	show_tree(find_item_by_id(items, 4051));
	printf("\n");

	print_item(find_item_by_id(items, 4051));
	//need to free array of items
	return 0;
}

void show_tree(Item *root)
{
    Set *queue;
    Set *components, *products, *summary, *surplus;
    Item *unit;
    Rcomponent *component, *product, *addable, *summary_unit, *surplus_comp;
    int index;
    bool test;

    queue = init_set();
    summary = init_set();
    surplus = init_set();
    set_insert(summary, create_rcomponent(root, 100));
    set_insert(queue, root);
    for(int i = 0; i < queue->size; i++)
    {
        unit = set_at_pos(queue, i);
        components = get_components(unit, 0);
        products = get_products(unit, 0);
        if(components && products)
        {
            product = set_at_pos(products, 0);
            //printf("\nproduct name: %s\n", unit->name);
            for(int j = 0; j < components->size; j++)
            {
                component = set_at_pos(components, j);
                //printf("%s: %d product %d ", component->item->name, component->quantity, product->quantity);
                test = set_insert_check(queue, component->item, &check_item);

                summary_unit = set_binary_search(summary, product, &rcomponent_sort);
                index = set_search_index(summary, component, &rcomponent_sort);
                if(index >= 0)
                {
                    addable = set_at_pos(summary, index);
                    //printf("exist %d summary ", addable->quantity);
                    addable->quantity += component->quantity * div_up(summary_unit->quantity, product->quantity);

                }
                else
                {
                    //printf("DEXIST %d summary ", summary_unit->quantity);
                    addable = create_rcomponent(component->item, component->quantity * div_up(summary_unit->quantity, product->quantity));
                    s_set_insert(summary, addable, &check_rcomponent,&rcomponent_sort);
                }
                //printf("%d\n", addable->quantity);
            }
        }
    }

    printf("Product: %s\n", root->name);
    for(int i = 0; i < summary->size; i++)
    {
        component = set_at_pos(summary, i);
        printf("%s %d\n", component->item->name, component->quantity);
        //if(component->item->recipes->size == 0)
        //    printf("%s %d\n", component->item->name, component->quantity);
    }
    printf("\nSurplus\n");
    for(int i = 0; i < summary->size; i++)
    {
        component = set_at_pos(summary, i);
        products = get_products(component->item, 0);
        if(products)
        {
            product = set_at_pos(products, 0);
            if(component->quantity % product->quantity != 0)
                printf("%s %d\n", component->item->name, product->quantity - component->quantity % product->quantity);
        }

    }
}

bool check_rcomponent(Set *summary, void *element)
{
    return !set_binary_search(summary, element, &rcomponent_sort);
}
bool check_item(Set *summary, void *element)
{
    Item *a;
    for(int i = 0; i < summary->size; i++)
    {
        a = set_at_pos(summary, i);
        if(a == element)
        {
            return false;
        }
    }
    return true;
}

int rcomponent_sort(void *arg1, void *arg2)
{
    Rcomponent *a, *b;
    a = arg1;
    b = arg2;
    return a->item->id - b->item->id;
}
int div_up(int x, int y)
{
    return (x-1)/y+1;
}

Rcomponent* create_rcomponent(Item *a, int q)
{
    Rcomponent *ret;
    ret = malloc(sizeof(Rcomponent));
    ret->item = a;
    ret->quantity = q;
    return ret;
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
	JSON *inp, *json_recipes,  *json_recipe, *json_products, *json_components;
	List *jrecipes;
	Recipe *unit;
	Set *rcomponents;
	Rcomponent *tmp;
	inp = json_parse_file("recipes.json");
	if(inp->type == type_Object)
	{
		/*json_Object equals List*/
		json_recipes = list_at_pos(inp->value.object, 0);
		jrecipes = json_recipes->value.list;
		for(int i = 0; i < jrecipes->size; i++)
		{
			json_recipe = list_at_pos(jrecipes, i);
			unit = malloc(sizeof(Recipe));
			if(json_recipe->type == type_Object)
			{
				json_products = list_at_pos(json_recipe->value.object, 1);
				json_components = list_at_pos(json_recipe->value.object, 0);
				if(strcmp(json_products->key, "products") == 0 && strcmp(json_components->key, "components") == 0)
				{
					rcomponents = hfunc(items, json_products);
					unit->products = rcomponents;
					rcomponents = hfunc(items, json_components);
					unit->components = rcomponents;
					for(int j = 0; j < unit->products->size; j++)
					{
                        tmp = set_at_pos(unit->products, j);
                        if(tmp->item)
                        {
                            //printf("product id %d\n", tmp->item->id);
                            darray_append(tmp->item->recipes, unit);
                            //printf("after append\n");
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

Set* hfunc(Darray *items, JSON *materials)
{
    Set *result;
    Rcomponent *unit;
    JSON *json_rcomponent, *json_id, *json_quantity;
    List *materials_list;

    result = init_set();
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
                unit = malloc(sizeof(Rcomponent));
                unit->item = find_item_by_id(items, json_id->value.number);
                unit->quantity = json_quantity->value.number;
                set_insert(result, unit);
            }
            else
            {
                printf("rcomponent name error\n");
            }
        }
    }
    return result;
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
