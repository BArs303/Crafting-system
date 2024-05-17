#include "advanced_types.h"

#define DEFAULT_PRODUCT_QUANTITY 1

typedef struct item Item;
typedef struct recipe Recipe;
typedef struct recipe_component Rcomponent;

struct item
{
	int id;
	char *name;
	Set *used_in; //list of recipes
	Darray *recipes;
};

struct recipe
{
	Set *products;
	Set *components;
};

struct recipe_component
{
	Item *item;
	int quantity;
};


Item* create_item(int id, char *name);

void add_recipe(Item *a);
void add_recipe_with_quantity(Item *a ,int product_quantity);
void add_recipe_with_opts(Item *a ,int product_quantity, int num_of_components);

void add_product(Recipe *dest, Item *src, int product_quantity);
void add_component(Recipe *dest, Item *src, int component_quantity);


Recipe* get_recipe(Item *a, int index);
Set* get_components(Item *a, int recipe_index);
Set* get_products(Item *a, int recipe_index);

bool item_set_insert(Set* a, Item *element);

void print_item(Item *a);
void print_recipe(Recipe *a);

Item* convert_json_to_item(JSON *a);
JSON* convert_item_to_json(Item *a);
void unload_recipe(Recipe *recipe, FILE *ofs);

/*rewrite all functions
void destruct_item(Item *a);
void destruct_item_and_recipe(Item *a);//do not call this func
void remove_product_by_index(Recipe *a, int item_index);
void remove_product_by_id(Recipe *a, int id);
void destruct_recipe(Recipe *a);
void remove_recipe(Item *a, int index);*/

