#include "advanced_types.h"

#define DEFAULT_PRODUCT_QUANTITY 1

typedef struct item Item;
typedef struct recipe Recipe;
typedef struct recipe_component Rcomponent;

struct item
{
	int id;
	char *name;
	Darray *used_in; //list of recipes
	Darray *recipes;
};

struct recipe
{
	Darray *products;//list??
	Darray *components;
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

Recipe* get_recipe(Item *a, int index);
Darray* get_products(Item *a, int recipe_index);
Darray* get_components(Item *a, int recipe_index);

int id_comparison(void *a, void *b);
int rcomponent_comparison(void *a, void *b);


void print_item(Item *a);
void print_recipe(Recipe *a);

Item* convert_json_to_item(JSON *a);
JSON* convert_item_to_json(Item *a);
Rcomponent* convert_json_to_rc(Set *items, JSON *a);
Darray* convert_json_to_rca(Set *items, JSON *a);
Recipe* convert_json_to_recipe(Set *items, JSON *a);
