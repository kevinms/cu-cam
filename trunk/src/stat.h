/* 
 * File:   stat.h
 * Used by the server and client to send stat requests and handle them.
 */

#ifndef _STAT_H
#define	_STAT_H

#include "command.h"
#include "net.h"
#include "list.h"

#define ST_WHO  0  // Show who is logged on
#define ST_PROC 1  // Show process a user has running
#define ST_LS   2  // List the contents of a server

void stat_handle(int sock, struct command_t *cmd);
void stat_request(struct net_t *n, struct list_t *userName, char flag);

#endif	/* _STAT_H */

