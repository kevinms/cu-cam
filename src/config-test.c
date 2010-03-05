#include <stdio.h>

#include "config.h"

void print_client_groups();
void print_string_list(struct list_t *list);

int
main(int argc, char **argv)
{
	config_load("camserver.rc",CONFIG_SERVER);

	printf("camserver.rc\n");
	printf("----------------\n");
	printf("[server]\n");
	printf("server_port = %s\n",server->port);
	printf("server_ipv6 = \"%s\"\n",server->ipv6);
	printf("server_tls = \"%s\"\n",server->tls);
	printf("\n");

	config_load("camclient.rc",CONFIG_CLIENT);

	printf("camclient.rc\n");
	printf("----------------\n");
	printf("client_stat_interval = %d\n\n",client->stat_interval);
	print_client_groups();

	return 0;
}

void
print_client_groups()
{
	list_set_list_to_iterate(client->group);
	while(list_peek_link()!=NULL) {
		fprintf(stderr,"[group]\n");
		fprintf(stderr,"group_name = %s\n",((struct config_group_t *)list_peek_item())->name);
		fprintf(stderr,"group_servers = {\n");
		print_string_list(((struct config_group_t *)list_peek_item())->servers);
		fprintf(stderr,"}\n");
		fprintf(stderr,"group_users = {\n");
		print_string_list(((struct config_group_t *)list_peek_item())->users);
		fprintf(stderr,"}\n");
		fprintf(stderr,"\n");
		list_next_link();
	}
}

void
print_string_list(struct list_t *list)
{
	struct link_t *tmp;
	tmp = list->head;
	while(tmp!=NULL) {
		fprintf(stderr,"    %s\n",(char *)tmp->item);
		tmp = tmp->next;
	}
}
