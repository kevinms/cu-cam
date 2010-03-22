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

#include "config.h"
#include "utils.h"
#include "net.h"
#include "command.h"

void deamonize();

int
main(int argc, char **argv)
{
	struct net_t *n_tcp;
	struct net_t *n_udp;

	config_load("camserver.rc",CONFIG_SERVER); /* Load configuration file */
	if(server->deamon) deamonize(); /* run as a deamon */

	n_tcp = net_create_tcp_socket(NULL,server->port); /* Create a tcp socket to listen on */
	n_udp = net_create_udp_socket(NULL,server->port); /* Create a udp socket to listen on */

	/* fill fd_set with sockets */

	for(;;) {
		
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

	setsid(); /* obtain a new process group */
	umask(027); /* set newly created file permissions */

	if ((chdir("/")) < 0) exit(0);

	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP, SIG_IGN); /* catch hangup signal */
}
