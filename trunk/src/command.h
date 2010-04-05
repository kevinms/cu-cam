#ifndef COMMAND_H_
#define COMMAND_H_

#include "list.h"

#define CMD_ERR  0
#define CMD_GET  1
#define CMD_PUT  2
#define CMD_STAT 3

/*
 * +---+---+----+
 * |cmd|err|data|
 * +---+---+----+
 */

struct command_t {
	int type;
	int status;
	char *buf;
};

//possible wrapper functions for client requests
/*
//Sending, return -1 on fail to send 
int command_send_get(char *buf);
int command_send_put(char *buf);
int command_send_stat(char *buf);
*/

/* Parsing */
struct command_t *command_parse(char *buf);
char *command_parse_string(char **buf);
struct list_t *command_parse_list(char **buf);

/* Clean up */
void command_free(struct command_t *cmd);

#endif /* !COMMAND_H_ */
