#include "advanced_types.h"

struct inner_dict
{
	char *key;
	void *value;
	int type;
};

typedef struct inner_dict Pobject;

char* file_to_str(const char *filename);

char *get_line(char **buffer);
int level_skip(char **buffer, int lvl);

List *parse(char *str);
List *act_parse(char **buffer, int lvl);

char* get_key(char **buffer);
void* get_value(char **buffer, int lvl, int *type);

int read_until(char *str, char limiter);
bool is_whitespace(char c);
int skip_whitespaces(char **buffer, bool (*is_whitespace)(char));
char* copy_from_buffer(char **buffer, int length);