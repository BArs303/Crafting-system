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
Recipe* create_recipe();
Rcomponent* create_rcomponent(Item *a, int q);

bool add_component(Recipe *dst, Rcomponent *element);
bool add_product(Recipe *dst, Rcomponent *element);
void link_recipe(Recipe *a);

bool rcomponent_set_insert(Set *a, Rcomponent *element);
Rcomponent* rcomponent_find(Set *src, Rcomponent *element);
int rcomponent_find_index(Set *src, Rcomponent *element);

Recipe* get_recipe(Item *a, int index);
Set* get_products(Item *a, int recipe_index);
Set* get_components(Item *a, int recipe_index);

bool item_set_insert(Set *a, Item *element);

void print_item(Item *a);
void print_recipe(Recipe *a);

Item* convert_json_to_item(JSON *a);
JSON* convert_item_to_json(Item *a);
