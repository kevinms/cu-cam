#ifndef UTILS_H_
#define UTILS_H_

#include "list.h"

int split_string(char *buf, char **seg);
char *p_strcpy(char *str);
int search_match(char **tree, char *apple);
int list_lookup_string(struct list_t *list, char *name);
double getTime();
void die_with_error(char *error_message);

#endif /* !UTILS_H_ */
