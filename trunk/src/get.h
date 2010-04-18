/* 
 * File:   get.h
 * Used by the sever and client to send get requests and handle them.
 */



#ifndef _GET_H
#define	_GET_H

#include "net.h"
#include "list.h"

	void get_handle();
	int get_request(struct net_t *n, struct list_t *list, char *fileName, char *saveLoc);

#endif	/* _GET_H */
