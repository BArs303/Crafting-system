#include "interface.h"

void tmp_print(void *element)
{
	print_item(element);
}

int main()
{
	Set *items;
	/*Item *tmp;
	Rcomponent *r;
	*/
	HMap *table;

	/*test_request();*/
	table = init_hmap();
	items = general_load(table);
	if(!items)
		return -1;

	/*printf("ITEMS\n");
	items = load_items("items.json");
	printf("RECIPES\n");
	load_recipes(items, "recipes.json");
	printf("Loaded\n");*/

	interface_main(items, table);

	//delete_unused(items);
	/*tmp = create_item(11993, NULL);*/
	
	//printf("%p\n", set_find(items, &tmp, id_comparison));
	/*r = create_rcomponent(set_find(items, tmp, id_comparison), 1);*/
	//print_set(items, tmp_print);
	/*show_tree(r);
	printf("\n");*/
	delete_set(items, delete_item);

	//need to free array of items*/
	return 0;
}
