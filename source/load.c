#include "load.h"

static void soft_delete_json(void *element, void *params);
static void process_recipes(JSON *recipes, Set *items);
static void* load_items(void *fname);
static void* load_recipes(void *fname);

struct item_load_thread_parameters 
{
	char *fname;
	HMap *table;
};

Set* general_load(HMap *table)
{
	Set *items;
	JSON *recipes;
	pthread_t *tid;
	struct item_load_thread_parameters data;

	SetIterator *tmp;
	Item *current;
	unsigned int i;

	data.fname = "items.json";
	data.table = table;
	items = NULL;
	tid = malloc(sizeof(pthread_t) * 2);

	pthread_create(&tid[0], NULL, load_items, &data);
	pthread_create(&tid[1], NULL, load_recipes, "recipes.json");

	pthread_join(tid[0], (void**)&items);
	pthread_join(tid[1], (void**)&recipes);

	printf("Load completed\n");
	process_recipes(recipes, items);

	/*tmp = init_set_iterator(items);
	i = 0;
	while(si_has_next(tmp))
	{
		current = si_next(tmp);
		if(current->recipes->size > 0)
		{
			printf("item name: %s\nrecipes: %u i %u\n", current->data->name, current->recipes->size, i);
		}
		i++;
	}*/

	return items;
}
static void* load_recipes(void *fname)
{
	return json_parse_file(fname);
}

static void* load_items(void *parameters)
{
	JSON *a;
	HMap *object;
	HNode *field;
	List *items_list;
	struct item_load_thread_parameters *data;
	Item *item;
	Set *items = NULL;

	data = parameters;
	a = json_parse_file(data->fname);
	if(a)
	{
		object = a->value.object;
		items = init_set();
		items_list = hmap_to_list(object);
		for(int i = 0; i < items_list->size; i++)
		{
			field = list_at_pos(items_list, i);
			item = convert_json_to_item(field);

			set_insert(items, item, &id_comparison);
			hmap_insert(data->table, item->data->name, item);
		}
		delete_list(items_list, soft_delete_json, NULL);
		delete_hmap(object, passive_destruct, NULL);
		free(a);
	}
	return items;
}
static void soft_delete_json(void *element, void *params)
{
	JSON *a;
	a = element;
	if(a)
	{
		if(a->type == type_string)
		{
			free(element);
		}
	}
}

static void process_recipes(JSON *recipes, Set *items)
{
	List *jrecipes;
	JSON *json_recipes, *json_recipe;
	Recipe *t;
	if(recipes && recipes->type == type_object)
	{
		/*json object equals List. Replace to hashtable required*/
		json_recipes = hmap_get(recipes->value.object, "Recipes");
		jrecipes = json_recipes->value.list;
		printf("recipe size %u\n", jrecipes->size);
		for(int i = 0; i < jrecipes->size; i++)
		{
			json_recipe = list_at_pos(jrecipes, i);
			t = convert_json_to_recipe(items, json_recipe);
			if(t)
			{
				link_recipe(t);
			}
		}
		delete_json(recipes, NULL);
	}
}
