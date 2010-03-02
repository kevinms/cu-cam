/*
 *  Main Server ((used on each machine))
 *  planned to be "daemonized"
 */

#if 0
#include "config.h"
#include "command.h"
#include "utils.h"
#include "net.h"
#endif

int
main(int argc, char **argv)
{

#if 0
	config_load("camrc");    /* Load configuration file */
	net_create_tcp_socket(); /* Create a tcp socket to listen on */
	net_create_udp_socket(); /* Create a udp socket to listen on */
#endif

	for(;;) {
		
	}

	return 0;
}
