#ifndef CONFIG_H_
#define CONFIG_H_

#include "list.h"

struct config_pref_t
{
	struct config_server_pref_t *servers;
	struct config_client_pref_t *clients;
};

struct config_server_pref_t
{
	list_t *list;
};

struct config_client_pref_t
{
	list_t *list;
};

struct config_server_t
{
	char *port_TCP;
	char *port_UDP;
	char *ipv6;
	char *tls;
};

/* globals */
struct config_pref_t config_pref;

/* rc file types */
void config_load(char *rcfile);

/* section types */
struct config_server_t *config_parse_server(FILE *in);
struct config_client_pref_t *config_parse_client(FILE *in);

/* entry types */
void config_read_key(FILE *in);
list_t *config_read_list(FILE *in);
char *config_read_string(FILE *in);
char *config_read_value(FILE *in);

#endif /* !CONFIG_H_ */
