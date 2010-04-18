#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "net.h"
#include "utils.h"
#include "command.h"

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

	memset(&n->hints, 0, sizeof(n->hints)); /* make sure the struct is empty */
	n->hints.ai_family = AF_UNSPEC;        /* don't care IPv4 or IPv6 */
	n->hints.ai_socktype = SOCK_STREAM;    /* TCP stream sockets */
	n->hints.ai_flags = AI_PASSIVE;        /* fill in my IP for me */

	if ((status = getaddrinfo(node, service, &n->hints, &n->servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	n->sock = socket(n->servinfo->ai_family, n->servinfo->ai_socktype, n->servinfo->ai_protocol);

	int opt = 1;
	setsockopt(n->sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	setsockopt(n->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


	return n;
}

struct net_t *
net_create_udp_socket(char *node, char *service)
{
	struct net_t *n;
	int status;

	n = (struct net_t *)malloc(sizeof(*n));
	if(!n) {
		fprintf(stderr,"Error: Could not malloc() a new struct net *\n");
		return NULL;
	}

	memset(&n->hints, 0, sizeof(n->hints)); /* make sure the struct is empty */
	n->hints.ai_family = AF_UNSPEC;        /* don't care IPv4 or IPv6 */
	n->hints.ai_socktype = SOCK_DGRAM;    /* TCP stream sockets */
	n->hints.ai_flags = AI_PASSIVE;        /* fill in my IP for me */

	if ((status = getaddrinfo(node, service, &n->hints, &n->servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}

	n->sock = socket(n->servinfo->ai_family, n->servinfo->ai_socktype, n->servinfo->ai_protocol);

	return n;
	//return NULL;
}

int
net_connect(struct net_t *n)
{
	int rv;

	rv = connect(n->sock, n->servinfo->ai_addr, n->servinfo->ai_addrlen);

	return rv;
}

void
net_bind(struct net_t *n)
{
	if(bind(n->sock, n->servinfo->ai_addr, n->servinfo->ai_addrlen) < 0)
		die_with_error("bind() failed");
}

void
net_listen(struct net_t *n)
{
	/* Mark the socket so it will listen for incoming connections */
	if (listen(n->sock, MAXPENDING) < 0)
		die_with_error("listen() failed");
}

int
net_accept_tcp_client(int servSock)
{
	int clntSock;                    /* Socket descriptor for client */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned int clntLen;            /* Length of client address data structure */

	/* Set the size of the in-out parameter */
	clntLen = sizeof(echoClntAddr);

	/* Wait for a client to connect */
	if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
		die_with_error("accept() failed");

	return clntSock;
}

void
net_handle_tcp_client(int clntSocket)
{
	char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
	int recvMsgSize;                    /* Size of received message */
	int ack_msg_size = 16;
	char ack_msg[16];
	double t;

	memset(ack_msg,0,16);
	*(unsigned int *)(ack_msg) = htonl(16);
	

	/* Receive message from client */
	if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
		die_with_error("recv() failed");

	/* Send received string and receive again until end of transmission */
	while (recvMsgSize > 0)      /* zero indicates end of transmission */
	{
		t = getTime();
		*(unsigned int *)(ack_msg+4) = *(unsigned int *)(echoBuffer+4);
		*(double *)      (ack_msg+8) =  t;

		/* Echo message back to client */
		if (send(clntSocket, ack_msg, ack_msg_size, 0) != ack_msg_size)
			die_with_error("send() failed");

		/* See if there is more data to receive */
		if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
			die_with_error("recv() failed");
	}

	close(clntSocket);    /* Close client socket */
}

void
net_handle_udp_client(int clntSocket)
{
	char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
	int recvMsgSize;                 /* Size of received message */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned int cliAddrLen;         /* Length of incoming message */

	/* Set the size of the in-out parameter */
	cliAddrLen = sizeof(echoClntAddr);

	/* Block until receive message from a client */
	if ((recvMsgSize = recvfrom(clntSocket, echoBuffer, RCVBUFSIZE, 0,
		(struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
		die_with_error("recvfrom() failed");

	/* Send received datagram back to the client */
	if (sendto(clntSocket, echoBuffer, recvMsgSize, 0,
		 (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
		die_with_error("sendto() sent a different number of bytes than expected");
}

int
net_send_tcp(int sock, char *buf, int size)
{
	int dataSent = 0;

	/* Echo message back to client */
	if ((dataSent = send(sock, buf, size, 0)) != size)
		die_with_error("send() failed");

	fprintf(stderr,"SEND SENT: %d bytes\n",dataSent);
	return dataSent;
}

char *
net_recv_tcp(int sock)
{
	char *buf;
	int recvMsgSize;

	buf = (char *)malloc(RCVBUFSIZE);

	/* See if there is more data to receive */
	if ((recvMsgSize = recv(sock, buf, RCVBUFSIZE, 0)) < 0)
		die_with_error("recv() failed");

	fprintf(stderr,"RECV RECVED: %d bytes\n",recvMsgSize);

	return buf;
}

int
net_send_fragments_tcp(int sock, char *buf, int bufsize, int blocksize)
{
	char header[2];
	int hdrsize = 2;

	char send_buf[blocksize + hdrsize];
	int offset = 0;
	int count = 0;
	int rc;

	header[0] = CMD_GET;
	header[1] = STAT_MF;

	while(offset < bufsize && rc != -1) {
		if(offset + (blocksize - hdrsize) > bufsize) {
			blocksize -= (offset + (blocksize - hdrsize)) - bufsize;
			header[1] = STAT_EOF;
			fprintf(stderr,"setting STAT_EOF\n");
		}

		memcpy(send_buf,header,hdrsize);
		memcpy(send_buf,buf+offset,blocksize-hdrsize);
		fprintf(stderr,"offset: %d, bufsize: %d, blocksize: %d, count: %d\n",offset, bufsize, blocksize, count);

		rc = net_send_tcp(sock, send_buf, blocksize);
		offset += (blocksize-hdrsize);
		count += rc;
	}

	if(rc == -1)
		return rc;
	return count;
}

int
net_recv_fragments_tcp(int sock, char **buf, int bufsize)
{
	char data[RCVBUFSIZE];
	//char *buf;
	int rc;
	int count = 0;

	*buf = (char *)malloc(bufsize);
	data[1] = STAT_MF;

	while(data[1] == STAT_MF) {
		rc = recv(sock, data, RCVBUFSIZE, 0);
		if(rc < 0) {
			fprintf(stderr,"error: rc < 0\n");
			return rc;
		}

		fprintf(stderr,"rc: %d, count: %d, data[1]: %d\n",rc, count, data[1]);

		memcpy((*buf)+count,data+2,rc-2);
		count += rc;
		if(data[1] == STAT_EOF) {
			fprintf(stderr,"STAT_EOF recieved\n");
			return rc;
		}
	}
	return -1;
}

#if 0
void
net_sendto_udp(int sock, char *buf, int size)
{
	/* Send received datagram back to the client */
	if (sendto(clntSocket, echoBuffer, recvMsgSize, 0,
		(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
		die_with_error("sendto() sent a different number of bytes than expected");
}

char *
net_recvfrom_udp(struct net_t *n)
{

	char buf;     /* Buffer for echo string */
	int size;                 /* Size of received message */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned int cliAddrLen;         /* Length of incoming message */

	buf = (char *)malloc(RCVBUFSIZE);

	/* Set the size of the in-out parameter */
	cliAddrLen = sizeof(echoClntAddr);

	/* Block until receive message from a client */
	if ((size = recvfrom(clntSocket, buf, RCVBUFSIZE, 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
		die_with_error("recvfrom() failed");

	return buf;
}
#endif

void
net_free(struct net_t *n)
{
	close(n->sock);
	freeaddrinfo(n->servinfo);
	if(n->servIP) free(n->servIP);
	if(n) free(n);
}
