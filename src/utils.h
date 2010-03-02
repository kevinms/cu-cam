#ifndef _UTILS_H_
#define _UTILS_H_

#include "list.h"

int split_string (char *buf, char **seg);
char *p_strcpy (char *str);
int search_match(char **tree, char *apple);
int list_lookup_string(list_t *list, char *name);

#endif /* !_UTILS_H_ */
