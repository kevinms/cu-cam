#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <netinet/in.h>

#include "net.h"

struct net_t *
net_create_tcp_socket(char *node, char *service)
{

	struct net_t *n;
	int status;

	n = (struct net_t *)malloc(sizeof(*n));
	if(!n) {
		fprintf(stderr,"Error: Could not malloc() a new struct net *\n");
		return NULL;
	}

	memset(&n->hints, 0, sizeof n->hints); /* make sure the struct is empty */
	n->hints.ai_family = AF_UNSPEC;        /* don't care IPv4 or IPv6 */
	n->hints.ai_socktype = SOCK_STREAM;    /* TCP stream sockets */
	n->hints.ai_flags = AI_PASSIVE;        /* fill in my IP for me */

	if ((status = getaddrinfo(node, service, &n->hints, &n->servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	n->sock = socket(n->servinfo->ai_family, n->servinfo->ai_socktype, n->servinfo->ai_protocol);

	return n;
}

struct net_t *
net_create_udp_socket()
{
	return NULL;
}

int
net_connect(struct net_t *n)
{
	int rv;

	rv = connect(n->sock, n->servinfo->ai_addr, n->servinfo->ai_addrlen);

	return rv;
}

int
net_bind(struct net_t *n)
{
	int rv;

	rv = bind(n->sock, n->servinfo->ai_addr, n->servinfo->ai_addrlen);

	return rv;
}

void
net_free(struct net_t *n)
{
	close(n->sock);
	freeaddrinfo(n->servinfo);
}
