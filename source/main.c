#include "advanced_types.h"
#include "load.h"

Item* find_item_by_id(Darray *items, int id);
Item* item_binary_search(Darray *items, int id, int start, int end);

int div_up(int x, int y);

void show_tree(Rcomponent *root);
Rcomponent* form_scomponent(Item *a, int quantity);
int calculate_surplus(Item *a, int quantity);

void zeroLevelPrint(void *element);
void debugSurplusPrint(Set *surplus);

void update_surplus(Queue *queue, Set *surplus, Rcomponent *component, int quantity);
void update_summary(Set *summary, Rcomponent *required, Rcomponent *product);
void substract_surplus(Set *summary, Set *surplus);
void tmp_print(void *element)
{
	print_item(element);
}
int main()
{
	Set *items;
	Item tmp;
	Rcomponent *r;

	printf("ITEMS\n");
	items = load_items("items.json");
	printf("RECIPES\n");
	load_recipes(items, "recipes.json");
	printf("Loaded\n");

	tmp.id = 57457;
	//printf("%p\n", set_find(items, &tmp, id_comparison));
	r = create_rcomponent(set_find(items, &tmp, id_comparison), 10);
	//print_set(items, tmp_print);
	show_tree(r);
	printf("\n");

	//need to free array of items
	return 0;
}

void show_tree(Rcomponent *root)
{
	Queue *queue;
    Darray *components, *products;
    Set *summary, *surplus;
    Rcomponent *component, *product, *required;
	Rcomponent *addable, *surplus_component, *csummary;
	int quantity;

    queue = init_queue();
    summary = init_set();
    surplus = init_set();
	queue_push(queue, root);

	printf("summary %p\n", summary);
	while(!queue_is_empty(queue))
    {
		printf("queue size %d\n", queue->size);
        required = queue_pop(queue);
		printf("required %p\n", required);
        components = get_components(required->item, 0);
        products = get_products(required->item, 0);
        if(components && products)
        {
            product = darray_at_pos(products, 0);
			printf("product %s\n", product->item->name);
            for(int j = 0; j < components->size; j++)
            {
                component = darray_at_pos(components, j);
				printf("component %s %d\n", component->item->name, component->item->id);
				quantity = component->quantity * div_up(required->quantity, product->quantity);
				//surplus_component = rcomponent_find(surplus, component);//??
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
	//substract_surplus(summary, surplus);

	//printf("Queue\n");
	//debugQueuePrint(queue, 0);
	printf("Summary\n");
	print_set(summary, &zeroLevelPrint);
	printf("Surplus\n");
	debugSurplusPrint(surplus);
}

/*void substract_surplus(Set *summary, Set *surplus)//need to rework
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
}*/

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

	csummary = set_find(summary, required, &rcomponent_comparison);
	if(csummary)
	{
		csummary->quantity += quantity;
	}
	else
	{
		set_insert(
		summary,
		create_rcomponent(required->item, quantity),
		&rcomponent_comparison);
	}
	return;
}

void update_surplus(Queue *queue, Set *surplus, Rcomponent *component, int quantity)
{
	Rcomponent *surplus_component, *addable;
	int s;

	surplus_component = set_find(surplus, component, &rcomponent_comparison);
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
			queue_push(queue, addable);
		}
	}
	else
	{
		//printf("just adding\n");
		addable = create_rcomponent(component->item, quantity);
		if(s != 0)
		{
			set_insert(
			surplus,
			create_rcomponent(component->item, s),
			&rcomponent_comparison);
		}
		queue_push(queue, addable);
	}
	return;
}
void zeroLevelPrint(void *element)
{
	Rcomponent *a;
	a = element;
	if(a->item->recipes->size == 0)
	{
		printf("%s %d\n", a->item->name, a->quantity);
	}
}
void debugSurplusPrint(Set *surplus)
{
	return;
}

int calculate_surplus(Item *a, int quantity)
{
	Darray *products;
	Rcomponent *product;
	int result = 0;
	products = get_products(a, 0);
	if(products)
	{
		product = darray_at_pos(products, 0);
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

