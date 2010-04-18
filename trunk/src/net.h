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

#define RCVBUFSIZE 10016 /* Size of receive buffer */
#define MAXPENDING 5     /* Maximum outstanding connection requests */

/* setup a socket */
struct net_t *net_create_tcp_socket(char *node, char *service);
struct net_t *net_create_udp_socket(char *node, char *service);

/* server bind to port */
void net_bind(struct net_t *n);

/* server list on port */
void net_listen(struct net_t *n);

/* client connect to tcp server */
int net_connect(struct net_t *n);

/* server accept tcp client */
int net_accept_tcp_client(int fd);

/* handle tcp & udp clients */
void net_handle_tcp_client(int fd);
void net_handle_udp_client(int fd);

/* send/recv tcp */
int net_send_tcp(int sock, char *buf, int size);
char *net_recv_tcp(int sock);

/* send/recv udp */
void net_sendto_udp(struct net_t *n);
char *net_recvfrom_udp(struct net_t *n);

/* send smaller fragments of data each with the specified header*/
int net_send_fragments_tcp(int sock, char *buf, int bufsize, int blocksize);
int net_recv_fragments_tcp(int sock, char **buf, int bufsize);

/* free struct net_t */
void net_free(struct net_t *n);

#endif /* !NET_H_ */
