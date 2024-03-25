#include "item.h"
#include "json.h"

Darray* items_creation();

int main()
{
	JSON *a = json_parse_file("recipes.txt");
	printf("%s\n", json_stringify(a));
	/*Darray *items;
	Item *a;
	items = items_creation();
	a = darray_at_pos(items, 0);
	add_recipe(a);
	print_darray(items, &print_element);*/

	

	//need to free array of items
	return 0;
}

/*Darray* items_creation()
{
	Item *element;
	Pobject *tmp;
	List *parsed_list;
	char *str;
	Darray *items;
	int id = 0;

	str = file_to_str("items.txt");
	parsed_list = parse(str);
	free(str);

	items = init_darray_with_length(parsed_list->size);
	for(int i = 0; i < parsed_list->size; i++)
	{
		tmp = list_at_pos(parsed_list, i);
		if(tmp->type == TYPE_String)
		{
			id = str_to_int(tmp->key);
			element = create_item(id, tmp->value);
			darray_append(items, element);
		}
	}

	//need to free parsed_list
	return items;
}*/
