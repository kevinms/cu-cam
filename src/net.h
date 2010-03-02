#ifndef NET_H_
#define NET_H_

int net_create_tcp_socket();
int net_create_udp_socket();
int net_accept_tcp_client(int fd);
void net_handle_tcp_client(int fd);
void net_handle_udp_client(int fd);

#endif /* !NET_H_ */
