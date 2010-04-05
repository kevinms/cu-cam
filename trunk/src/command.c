#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"
#include "utils.h"

struct command_t *
command_parse(char *buf)
{
	struct command_t *c;
	c = (struct command_t *)malloc(sizeof *c);
	if(!c) return NULL;

	c->type = *(char *)buf;
	c->status = *(((char *)buf)+1);
	c->buf = buf+2;

	return c;
}

char *
command_parse_string(char **buf)
{
	int size;
	char *str;

	if(**buf == ',' || **buf == '!')
		(*buf)++;

	size = strcspn(*buf,",!");
	if(size == 0)
		return NULL;

	str = (char *)malloc(size);
	strcpy(str,*buf);
	str[size] = '\0';

	(*buf) += size;
	return str;
}

struct list_t *
command_parse_list(char **buf)
{
	char *str;
	struct list_t *l;
	l = list_init();

	while((str = command_parse_string(buf)) != NULL) {
			list_add(l,str);
		if(**buf == '!')
			return l;
	}
	
	return l;
}

void
command_free(struct command_t *cmd)
{
	if(cmd) {
		free(cmd);
		cmd = NULL;
	}
}
