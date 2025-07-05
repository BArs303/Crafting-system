#include "json.h"
#include "dynamic_array.h"
#include "set.h"

#ifndef ITEMS_H
#define ITEMS_H
#define DEFAULT_PRODUCT_QUANTITY 1

typedef struct item_data IData;
typedef struct recipe_data RData;

typedef struct item Item;
typedef struct recipe Recipe;
typedef struct recipe_component Rcomponent;

struct item_data
{
	char *name;
	float price;
};

struct recipe_data
{
	short material_efficiency;
};

struct item
{
	int id;
	IData *data;
	Darray *used_in; /*list of recipes*/
	Darray *recipes;
};

struct recipe
{
	RData *data;
	Darray *products;/*list??*/
	Darray *components;
};

struct recipe_component
{
	Item *item;
	int quantity;
};


Item* create_item(int id, char *name);
void delete_item(void *item, void *parameters);

Recipe* create_recipe();
void delete_recipe(void *recipe);
Rcomponent* create_rcomponent(Item *a, int q);
void delete_rcomponent(void *rcomponent, void *params);


bool add_component(Recipe *dst, Rcomponent *element);
bool add_product(Recipe *dst, Rcomponent *element);

void link_recipe(Recipe *a);
void unlink_recipe(Recipe *a);


int id_comparison(void *a, void *b);
int rcomponent_comparison(void *a, void *b);

Darray* get_components(Item *a, unsigned int recipe_number);
Darray* get_products(Item *a, unsigned int recipe_number);

void print_item(void *a);
void print_recipe(Recipe *a);

Item* convert_json_to_item(HNode *a);
JSON* convert_item_to_json(Item *a);

Rcomponent* convert_json_to_rc(Set *items, JSON *a);
Darray* convert_json_to_rca(Set *items, JSON *a);
Recipe* convert_json_to_recipe(Set *items, JSON *a);
#endif
