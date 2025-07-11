#include "mycalc.h"

int div_up(int x, int y);

static int calculate_surplus(Item *a, int quantity);

static void zeroLevelPrint(void *element, void *params);
static void debugSurplusPrint(void *element, void *params);
static void clear_summary(void *element, void *params);

static void update_surplus_and_queue(Queue *queue, Set *surplus, Rcomponent *component, int quantity);
static void update_summary(Set *summary, Rcomponent *required, Rcomponent *product);
static void substract_surplus(Set *summary, Set *surplus);

Set* show_tree(Rcomponent *root)
{
	Queue *queue;
	Darray *components, *products;
	Set *summary, *surplus;
	Rcomponent *component, *product, *required;
	int quantity;

	queue = init_queue();
	summary = init_set();
	surplus = init_set();
	update_surplus_and_queue(queue, surplus, root, root->quantity);

	while(!queue_is_empty(queue))
	{
		/*printf("queue size %d\n", queue->size);*/
		required = queue_pop(queue);
		/*printf("required %p\n", required);*/
		components = get_components(required->item, 0);
		products = get_products(required->item, 0);
		if(components && products)
		{
			product = darray_at_pos(products, 0);
			/*printf("product %s %d\n", product->item->data->name, product->quantity);*/
			for(int j = 0; j < components->size; j++)
			{
				component = darray_at_pos(components, j);
				/*printf("component %s %d\n", component->item->data->name, component->quantity);*/
				quantity = component->quantity * div_up(required->quantity, product->quantity);
				/*surplus_component = rcomponent_find(surplus, component);//??*/
				update_surplus_and_queue(queue, surplus, component, quantity);

			}
			/*printf("\n");*/
			update_summary(summary, required, product);
		}
		else
		{
			update_summary(summary, required, NULL);
		}
	}
	substract_surplus(summary, surplus);

	/*printf("Queue\n");*/
	/*debugQueuePrint(queue, 0);*/
	printf("Summary\n");
	print_set(summary, &zeroLevelPrint, NULL);
	printf("Surplus\n");
	print_set(surplus, &debugSurplusPrint, NULL);
	delete_set(summary, clear_summary, NULL);
	delete_set(surplus, clear_summary, NULL);
	return summary;
}

void clear_summary(void *element, void *params)
{
	free(element);
}


void substract_surplus(Set *summary, Set *surplus)
{
	Rcomponent *unit, *sunit;
	SetIterator *surplus_iterator;
	surplus_iterator = init_set_iterator(surplus);
	while(si_has_next(surplus_iterator))
	{
		unit = si_next(surplus_iterator);
		sunit = set_find(summary, unit, &rcomponent_comparison);
		sunit->quantity -= unit->quantity;
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

void update_surplus_and_queue(Queue *queue, Set *surplus, Rcomponent *component, int quantity)
{
	Rcomponent *surplus_component, *addable;
	int s;

	surplus_component = set_find(surplus, component, &rcomponent_comparison);
	s = calculate_surplus(component->item, quantity);
	/*printf("%s need %d ", component->item->name, quantity);*/

	if(surplus_component)
	{
		/*printf("already exists with quantity %d ", surplus_component->quantity);*/
		if(surplus_component->quantity > quantity)
		{
			/*printf("do not add\n");*/
			surplus_component->quantity -= quantity;
		}
		else
		{
			quantity -= surplus_component->quantity;
			surplus_component->quantity = calculate_surplus(component->item, quantity);
			/*printf("add to queue with %d\n", quantity);*/
			addable = create_rcomponent(component->item, quantity);
			queue_push(queue, addable);
		}
	}
	else
	{
		/*printf("just adding\n");*/
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
void zeroLevelPrint(void *element, void *params)
{
	Rcomponent *a;
	a = element;
	printf("%s %d\n", a->item->data->name, a->quantity);
	/*if(a->item->recipes->size == 0)
	{
		printf("%s %d\n", a->item->data->name, a->quantity);
	}*/
}
void debugSurplusPrint(void *b, void *params)
{
	Rcomponent *a;
	a = b;
	if(a->quantity > 0)
		printf("%s %d\n", a->item->data->name, a->quantity);
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

