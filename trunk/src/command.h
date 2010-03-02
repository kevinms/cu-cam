#ifndef COMMAND_H_
#define COMMAND_H_

#include "list.h"

/*
 * Command Examples:
 * 
 * GET!<user>:<file|folder>{,<file|folder>}
 * PUT!<user>{,<user>}|all:<file|folder>{,<file|folder>}
 * STAT!<user>{,<user>}|all
 * 
 * foo             const string
 * <foo>           parameter
 * [<foo>]         optional parameter
 * <foo>{,<foo>}   list of parameters
 * [<foo>{,<foo>}] optional list of parameters
 * <foo|bar>       either 'foo' or 'bar'
 * 
 * GET!alice:asgn1,asgn2/auth.c
 * PUT!all:asg1.tar.gz
 * STAT!fred,george,sally
 * STAT!all
 */

#include "list.h"

#define CMD_GET  1
#define CMD_PUT  2
#define CMD_STAT 3

struct command_t {
	int type;
	struct list_t *users;
	struct list_t *files;
};

/* Parsing */
struct command_t *command_parse(char *buf);
int command_parse_get(struct command_t *cmd, char *par);
int command_parse_put(struct command_t *cmd, char *par);
int command_parse_stat(struct command_t *cmd, char *par);
char *command_parse_list(struct list_t **list, char *buf);

/* Clean up */
void command_free(struct command_t *cmd);

#endif /* !COMMAND_H_ */
