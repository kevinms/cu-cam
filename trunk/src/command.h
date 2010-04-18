#ifndef COMMAND_H_
#define COMMAND_H_

#include "list.h"

#define CMD_ERR  0
#define CMD_GET  1
#define CMD_PUT  2
#define CMD_STAT 3

/* Status Codes */
/*
Not enough disk space for file!
Incorrect Permissions (folder or file)
Malformed Request From Client
File Send Size is <= 0
User/File In Request Not On Server
Unknown Stat Type

Incorrect Permissions (folder or file)
Malformed Request From Server
Recieve Error Notification From Server
*/
#define STAT_OK        0  // Everyting is ok
#define STAT_MF        1  // More file fragements (used by get and put)
#define STAT_EOF       2  // End of file|finished sending file
#define STAT_NO_SPACE  3  //ERR Not enough disk space for file
#define STAT_BAD_PERM  4  //ERR Bad permissions for file
#define STAT_MALF_REQ  5  //ERR Malformed request|packet
#define STAT_BAD_SIZE  6  //ERR Bad file size <= 0
#define STAT_NOS_FILE  7  //ERR No such file
#define STAT_NOS_USER  8  //ERR No such user
#define STAT_UNK_STAT  9  //ERR Unkown stat type
#define STAT_ERROR     10 //ERR General error notification

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
