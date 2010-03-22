#ifndef NET_H_
#define NET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct net_t {
	int sock;                  /* Socket file descriptor */
	unsigned short port;       /* Port number*/
	struct addrinfo hints;     /* Address info IP version agnostic*/
	struct addrinfo *servinfo; /* Filled after a getaddrinfo() */
	struct timeval tv;         /* */
	char *servIP;              /* IP address of server */

	struct net_t *next;
};

/* setup a socket */
struct net_t *net_create_tcp_socket(char *node, char *service);
struct net_t *net_create_udp_socket();

/* server bind to port */
int net_bind(struct net_t *n);

/* client connect to tcp server */
int net_connect(struct net_t *n);

/* server accept tcp client */
int net_accept_tcp_client(int fd);

/* handle tcp & udp clients */
void net_handle_tcp_client(int fd);
void net_handle_udp_client(int fd);

/* free struct net_t */
void net_free(struct net_t *n);

#endif /* !NET_H_ */
