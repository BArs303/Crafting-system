#include "load.h"

Set* load_items(const char *fname)
{
	JSON *a, *field;
	List *object;
	Set *items = NULL;
	a = json_parse_file(fname);
	if(a)
	{
		object = a->value.object;
		items = init_set();
		for(int i = 0; i < object->size; i++)
		{
			field = list_at_pos(object, i);
			set_insert(items, convert_json_to_item(field), &id_comparison);
		}
	}
	return items;
}

void load_recipes(Set *items, const char *fname)
{
	JSON *a, *json_recipes, *json_recipe;
	List *jrecipes;
	Recipe *t;
	a = json_parse_file(fname);
	if(a && a->type == type_Object)
	{
		/*json object equals List. Replace to hashtable required*/
		json_recipes = list_at_pos(a->value.object, 0);//first field of a is list of recipes
		jrecipes = json_recipes->value.list;
		for(int i = 0; i < jrecipes->size; i++)
		{
			json_recipe = list_at_pos(jrecipes, i);
			t = convert_json_to_recipe(items, json_recipe);
			if(t)
				link_recipe(t);
		}
	}
	return;
}
