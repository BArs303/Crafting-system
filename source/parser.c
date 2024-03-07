#include "parser.h"

static int define_level(char *str, int tab_size);

char* file_to_str(const char *fn)
{
	FILE *ifs;
	int length;
	char *str;
	ifs = fopen(fn, "rb");
	if(ifs)
	{
		fseek(ifs, 0, SEEK_END);
		length = ftell(ifs);
		fseek(ifs, 0, SEEK_SET);
		str = malloc(sizeof(char) * length);
		fread(str, 1, length, ifs);
	}
	fclose(ifs);
	return str;
}

char* get_key(char **buffer)
{
	char *key;
	int length;
	length = read_until(*buffer, ':');
	key = copy_from_buffer(buffer, length);
	return key;
}

void* get_value(char **buffer, int lvl, int *type)
{
	void *value;
	int length;
	length = read_until(*buffer, '\n');
	if(length == 0)
	{
		(*buffer)++;
		value = act_parse(buffer, lvl+1);
		*type = TYPE_Struct;
	}
	else
	{
		value = copy_from_buffer(buffer, length);
		*type = TYPE_String;
	}
	return value;
}

char *copy_from_buffer(char **buffer, int length)
{
	char *value;
	value = malloc(sizeof(char) * (length + 1));
	for(int i = 0; i < length; i++, (*buffer)++)
	{
		value[i] = **buffer;
	}
	value[length] = 0;
	(*buffer)++;
	return value;
}

List* parse(char *str)
{
	List *a;
	char **buffer;
	buffer = &str;
	a = act_parse(buffer, 0);
	return a;
}

List* act_parse(char **buffer, int lvl)
{
	Pobject *obj;
	List *data;
	int type, stop, tab_size;
	stop = 0;
	tab_size = 4;

	data = init_list();

	while(**buffer && define_level(*buffer, tab_size) == lvl)
	{
		obj = malloc(sizeof(Pobject));
		(*buffer) += tab_size * lvl;
		obj->key = get_key(buffer);

		skip_whitespaces(buffer, &is_whitespace);
		obj->value = get_value(buffer, lvl, &type);
		obj->type = type;
		if(lvl == 0)
		{
			stop++;
		}
		list_append(data, obj);
	}
	if(lvl == 0)
		printf("in parser %d\n", stop);
	return data;
}

int read_until(char *str, char limiter)
{
	char *tmp = str;
	while(*str && *str != limiter)
	{
		str++;
	}
	return str-tmp;
}

static int define_level(char *str, int tab_size)
{
	int ret = 0;
	if(str)
	{
		while(*str && *str == ' ')
		{
			str++;
			ret++;
		}
		if(*str == '-')
		{
			ret+=tab_size;
		}
		return ret%tab_size == 0 ? ret/tab_size:-1;
	}
	return -1;
}
char* get_list(char **buffer)
{
	int length;
	char *line;
	length = read_until(*buffer, '\n');
	line = malloc(sizeof(char) * (length+1));
	for(int i = 0; i < length; i++, (*buffer)++)
	{
		line[i] = **buffer;
	}
	(*buffer)++; //skip \n char
	line[length] = 0;
	return line;
}

int skip_whitespaces(char **buffer, bool (*is_whitespace)(char))
{
	int i;
	for(i = 0; is_whitespace(**buffer) && **buffer; i++, (*buffer)++)
	{}
	return i;
}

bool is_whitespace(char c)
{
	int n = 1;
	char pattern[] = {' '};
	for(int i = 0; i < n; i++)
	{
		if(c == pattern[i])
			return 1;
	}
	return 0;
}