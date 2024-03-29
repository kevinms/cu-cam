#ifndef UTILS_H_
#define UTILS_H_

#include "list.h"

#define EN_BIG 0
#define EN_LIT 1

int split_string(char *buf, char **seg);
char *p_strcpy(char *str);
int search_match(char **tree, char *apple);
int list_lookup_string(struct list_t *list, char *name);
double getTime();
void die_with_error(char *error_message);
int fsize(const char *f);
int fcheck_for_user(const char *user);
int fcheck_for_file(const char *file);
char *fileFromPath(char * path);

#endif /* !UTILS_H_ */
