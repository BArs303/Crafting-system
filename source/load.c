#include "load.h"

static void soft_delete_json(void *element);
static void process_recipes(JSON *recipes, Set *items);
static void* load_items(void *fname);
static void* load_recipes(void *fname);

Set* general_load()
{
	Set *items;
	JSON *recipes;
	pthread_t *tid;

	items = NULL;
	tid = malloc(sizeof(pthread_t) * 2);

	pthread_create(&tid[0], NULL, load_items, "items.json");
	pthread_create(&tid[1], NULL, load_recipes, "recipes.json");

	pthread_join(tid[0], (void**)&items);
	pthread_join(tid[1], (void**)&recipes);

	process_recipes(recipes, items);

	return items;
}
static void* load_recipes(void *fname)
{
	return json_parse_file(fname);
}

static void* load_items(void *fname)
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
		delete_list(object, soft_delete_json);
		free(a);
	}
	return items;
}

static void soft_delete_json(void *element)
{
	JSON *a;
	a = element;
	if(a)
	{
		if(a->type == type_String)
		{
			free(a->key);
			free(element);
		}
	}
}

static void process_recipes(JSON *recipes, Set *items)
{
	List *jrecipes;
	JSON *json_recipes, *json_recipe;
	Recipe *t;
	if(recipes && recipes->type == type_Object)
	{
		/*json object equals List. Replace to hashtable required*/
		json_recipes = list_at_pos(recipes->value.object, 0);//first field of a is list of recipes
		jrecipes = json_recipes->value.list;
		for(int i = 0; i < jrecipes->size; i++)
		{
			json_recipe = list_at_pos(jrecipes, i);
			t = convert_json_to_recipe(items, json_recipe);
			if(t)
				link_recipe(t);
		}
		delete_json(recipes);
	}
}
