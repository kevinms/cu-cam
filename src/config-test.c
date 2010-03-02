#include <stdio.h>
#include <dlfcn.h>

#include "config.h"

int
main(int argc, char **argv)
{
	config_load("camrc");

	struct config_server_t *s = (struct config_server_t *)config_pref.servers->list->head->item;
	struct config_client_pref_t *p = (struct config_client_pref_t *)config_pref.clients;
	printf("Conf Preferences\n");
	printf("----------------\n");
	printf("[server]\n");
	printf("server_port_TCP = %s\n",s->port_TCP);
	printf("server_port_UDP = %s\n",s->port_UDP);
	printf("server_ipv6 = \"%s\"\n",s->ipv6);
	printf("server_tls = \"%s\"\n",s->tls);
	printf("\n");

	link_t *tmp = p->list->head;
	printf("[client]\n");
	printf("server_list = {\n");
	while(tmp != NULL) {
		printf("\t%s\n",(char *)tmp->item);
		tmp = tmp->next;
	}
	printf("}\n");

	return 0;
}
