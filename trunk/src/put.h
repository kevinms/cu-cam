/* 
 * Used by the server and client to send put requests and handle them.
 *
 */

#ifndef _PUT_H
#define	_PUT_H

    void put_handle(int sock, struct command_t *cmd);
    int put_request(struct net_t *n, struct list_t *list, char *fileName, char *saveLoc);

#endif	/* _PUT_H */

