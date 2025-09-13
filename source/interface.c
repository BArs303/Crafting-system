#include "interface.h"

void interface_main(Set *items, HMap *table)
{
	Rcomponent *selected_item;
	unsigned int psize;
	size_t size, csize;
	ssize_t nread;
	char buffer[100];
	char *command, *item_name;
	char **parameters;
	size = 100;
	selected_item = create_rcomponent(NULL, 0);

	while(true)
	{
		nread = read(0, buffer, size);
		if(nread == -1)
			printf("User read error in interface function\n");
		buffer[nread-1] = 0;
		parameters = str_split(buffer, " ", &psize);
		command = parameters[0];
		if(strcmp(command, "exit") == 0)
			break;
		else if(strcmp(command, "select") == 0)
		{
			csize = strlen(command);
			while(is_whitespace(buffer[csize], " ") && buffer[csize])
			{
				csize++;
			}
			item_name = malloc(sizeof(char) * (size - csize));
			*item_name = 0;
			item_name = strcat(item_name, buffer+csize);

			selected_item->item = hmap_get(table, item_name);
			if(selected_item->item != NULL)
			{
				printf("selected id: %d name: %s\n", selected_item->item->id, selected_item->item->data->name);
			}
			else
			{
				printf("item not found\n");
			}
		}
		else if(strcmp(command, "set") == 0)
		{
			selected_item->quantity = atoi(parameters[1]);
		}
		else if(strcmp(command, "show") == 0)
		{
			if(selected_item->item != NULL && selected_item->quantity != 0)
			{
				show_tree(selected_item);
			}
		}
	}
}
