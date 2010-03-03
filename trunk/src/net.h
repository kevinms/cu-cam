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

struct net_t *net_create_tcp_socket();
struct net_t *net_create_udp_socket();
int net_accept_tcp_client(int fd);
void net_handle_tcp_client(int fd);
void net_handle_udp_client(int fd);
void net_free(struct net_t *n);

#endif /* !NET_H_ */
