#include <stdio.h>
#include <dlfcn.h>

#include "config.h"

int
main(int argc, char **argv)
{
	config_load("pbuogtrc");

	struct config_server_t *s = (struct config_server_t *)config_pref.servers->list->head->item;
	struct config_plugin_pref_t *p = (struct config_plugin_pref_t *)config_pref.plugins;
	printf("Conf Preferences\n");
	printf("----------------\n");
	printf("[server]\n");
	printf("server_name = \"%s\"\n",s->name);
	printf("server_autoconnect = \"%s\"\n",s->autoconnect);
	printf("server_address = \"%s\"\n",s->address);
	printf("server_port = %s\n",s->port);
	printf("server_ipv6 = \"%s\"\n",s->ipv6);
	printf("server_ssl = \"%s\"\n",s->ssl);
	printf("server_password = \"%s\"\n",s->password);
	printf("server_nick1 = \"%s\"\n",s->nick1);
	printf("server_nick2 = \"%s\"\n",s->nick2);
	printf("server_nick3 = \"%s\"\n",s->nick3);
	printf("server_username = \"%s\"\n",s->username);
	printf("server_realname = \"%s\"\n",s->realname);
	printf("server_hostname = \"%s\"\n",s->hostname);
	printf("server_command = \"%s\"\n",s->command);
	printf("server_autojoin = \"%s\"\n",s->autojoin);
	printf("\n");

	link_t *tmp = p->list->head;
	printf("[plugin]\n");
	printf("plugin_autoload = %s\n",p->autoload);
	printf("plugin_list = {\n");
	while(tmp != NULL) {
		printf("\t%s\n",(char *)tmp->item);
		tmp = tmp->next;
	}
	printf("}\n");

	return 0;
}
