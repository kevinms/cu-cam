/*
 *  Main Server ((used on each machine))
 *  planned to be "daemonized"
 */

#include "config.h"
#include "utils.h"
#include "net.h"
#include "command.h"

int
main(int argc, char **argv)
{

	config_load("camrc");    /* Load configuration file */
	net_create_tcp_socket(); /* Create a tcp socket to listen on */
	net_create_udp_socket(); /* Create a udp socket to listen on */

	for(;;) {
		
	}

	return 0;
}
