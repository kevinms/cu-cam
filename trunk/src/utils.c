#include "utils.h"
#include "list.h"

int
p_strsplit(char *buf, char **seg)
{
	char *x;
	int i = 0;
	seg = NULL;

	x = strtok (buf, "\n");
	while (x != NULL) {
		i++;
		seg = (char **)realloc ((char *)seg, sizeof (char *));
		seg[i-1] = x;
		x = strtok (NULL, "\n");
	}

	return i;
}

char *
p_strcpy(char *str)
{
	char *new = malloc (sizeof (char) * strlen (str));
	strcpy (new, str);
	return new;
}

char *
p_strncpy(char *str, int len)
{
	char *new = malloc (sizeof (char) * len);
	strncpy (new, str, len);
	return new;
}

int
search_match(char **tree, char *apple)
{
	int i = 0;
	while (strcmp (apple, tree[i]) != 0) {
		i += 2;
		if (strcmp ("\0", tree[i]) == 0)
			return -1;
	}
	return i;
}

int
list_lookup_string(list_t *list, char *name)
{
	link_t *temp;

	if(list == NULL)
		return -1;

	temp = list->head;
	while (temp != NULL) {
		if (strstr (name, (char *)temp->item) != NULL)
			return 1;
		temp = temp->next;
	}

	return 0;
}
