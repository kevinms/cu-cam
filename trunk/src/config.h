#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>

#include "list.h"

struct config_group_t {
	char *name;
	struct list_t *servers;
	struct list_t *users;
};

struct config_server_t
{
	char *port;
	char *ipv6;
	char *tls;
};

struct config_client_t
{
	int stat_interval;

	struct list_t *group;
};

#define CONFIG_SERVER 0
#define CONFIG_CLIENT 1

/* globals */
extern struct config_server_t *server;
extern struct config_client_t *client;

/* rc file types */
void config_load(char *rcfile, int type);

/* config types */
struct config_server_t *config_parse_server(FILE *in);
struct config_client_t *config_parse_client(FILE *in);

/* section types */
struct config_group_t *config_parse_group(FILE *in);

/* entry types */
void config_read_key(FILE *in);
struct list_t *config_read_list(FILE *in);
char *config_read_string(FILE *in);
char *config_read_value(FILE *in);

#endif /* !CONFIG_H_ */
