#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "net.h"

struct net_t *
net_create_tcp_socket()
{

	struct net_t *n;
	int status;

	n = (struct net_t *)malloc(sizeof(*n));
	if(!n) {
		fprintf(stderr,"Error: Could not malloc() a new struct net *\n");
	}

	memset(&n->hints, 0, sizeof n->hints); /* make sure the struct is empty */
	n->hints.ai_family = AF_UNSPEC;        /* don't care IPv4 or IPv6 */
	n->hints.ai_socktype = SOCK_STREAM;    /* TCP stream sockets */
	n->hints.ai_flags = AI_PASSIVE;        /* fill in my IP for me */

	if ((status = getaddrinfo(NULL, "3490", &n->hints, &n->servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	return n;
}

struct net_t *
net_create_udp_socket()
{
	return NULL;
}

void
net_free(struct net_t *n)
{
	freeaddrinfo(n->servinfo);
}
