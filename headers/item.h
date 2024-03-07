#include "advanced_types.h"

#define DEFAULT_PRODUCT_QUANTITY 1
typedef struct item Item;
typedef struct recipe Recipe;
typedef struct rds Rds;

struct item
{
	int id;
	char *name;
	Set *used_in; //list of recipes
	Darray *recipes;
};

struct recipe
{
	Rds *products;
	Rds *components;
};

struct rds //recipe data storage
{
	Set *items;
	Darray *quantity;
};

Item* create_item(int id, char *name);
void destruct_item(Item *a);
void destruct_item_and_recipe(Item *a);//do not call this func

void add_recipe(Item *a);
void add_recipe_with_quantity(Item *a ,int product_quantity);
void add_recipe_with_opts(Item *a ,int product_quantity, int num_of_components);
void remove_recipe(Item *a, int index);
void destruct_recipe(Recipe *a);

void add_product(Recipe *dest, Item *src, int product_quantity);
void remove_product_by_index(Recipe *a, int item_index);
void remove_product_by_id(Recipe *a, int id);

void add_component(Recipe *dest, Item *src, int component_quantity);

Recipe* get_recipe(Item *a, int index);

void print_item(Item *a);
void print_recipe(Recipe *a);