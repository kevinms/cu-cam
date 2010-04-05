/*
 *  Main Server ((used on each machine))
 *  planned to be "daemonized"
 */

#include <signal.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#include "config.h"
#include "utils.h"
#include "net.h"
#include "command.h"

void deamonize();
void child_exit_signal_handler();

unsigned int child_proc_count = 0;

int
main(int argc, char **argv)
{
	int maxDescriptor;         /* Maximum socket descriptor value */
	fd_set sockSet;            /* Set of socket descriptors for select() */
	int client_sock;
	struct net_t *n_tcp;
	struct net_t *n_udp;
	pid_t pid;           /* Process ID from fork() */

	config_load("camserver.rc",CONFIG_SERVER); /* Load configuration file */
	if(server->deamon) deamonize();            /* Run as a deamon */

	n_tcp = net_create_tcp_socket(NULL,server->port); /* Create a tcp socket to listen on */
	n_udp = net_create_udp_socket(NULL,server->port); /* Create a udp socket to listen on */

	net_bind(n_tcp);
	net_bind(n_udp);

	net_listen(n_tcp);

	/* Determine if new descriptor is the largest */
	n_tcp->sock > n_udp->sock ? (maxDescriptor = n_tcp->sock) : (maxDescriptor = n_udp->sock);

	for(;;) {
		/* Zero socket descriptor vector and set for server sockets */
		/* This must be reset every time select() is called */
		FD_ZERO(&sockSet);

		/* Add keyboard to descriptor vector */
		FD_SET(n_tcp->sock, &sockSet);
		FD_SET(n_udp->sock, &sockSet);

		/* Suspend program until descriptor is ready or timeout */
		if (select(maxDescriptor + 1, &sockSet, NULL, NULL, NULL) == 0)
			printf("Server still alive\n");
		else if (FD_ISSET(n_tcp->sock, &sockSet)) {
			client_sock = net_accept_tcp_client(n_tcp->sock);

			if (client_sock < 0)
				continue;
			/* Fork child process and report any errors */
			if ((pid = fork()) < 0)
				die_with_error("fork() failed");
			else if (pid == 0) { /* If this is the child process */
				net_free(n_tcp); /* Child closes parent socket file descriptor */
				net_handle_tcp_client(client_sock);
				exit(0);         /* Child process done */
			}

			close(client_sock); /* Parent closes child socket descriptor */
			child_proc_count++; /* Increment number of outstanding child processes */
		}
		else if (FD_ISSET(n_udp->sock, &sockSet))
			net_handle_udp_client(n_udp->sock);
	}

	return 0;
}

void
deamonize()
{
	int i;

	if(getppid()==1) return; /* already a daemon */

	i=fork();

	if (i<0) exit(1); /* fork error */
	if (i>0) exit(0); /* parent exits */

	setsid();   /* obtain a new process group */
	umask(027); /* set newly created file permissions */

	if ((chdir("/")) < 0) exit(0);

	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP, SIG_IGN); /* catch hangup signal */
}

/* Clean up all zombies */
void
child_exit_signal_handler()
{
	pid_t pid; /* Process ID from fork() */

	while (child_proc_count) {
		/* Non-blocking wait */
		pid = waitpid((pid_t) -1, NULL, WNOHANG);

		/* waitpid() error? */
		if (pid < 0)  
			die_with_error("waitpid() failed");
		/* No child to wait on */
		else if (pid == 0)
			break;
		/* Cleaned up after a child */
		else
			child_proc_count--;
	}
}
