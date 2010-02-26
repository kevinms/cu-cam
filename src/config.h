#ifndef CONFIG_H_
#define CONFIG_H_

#include "list.h"

struct config_pref_t
{
	struct config_server_pref_t *servers;
	struct config_plugin_pref_t *plugins;
};

struct config_server_pref_t
{
	list_t *list;
};

struct config_plugin_pref_t
{
	char *autoload;
	list_t *list;
};

struct config_server_t
{
	char *name;
	char *autoconnect;
	char *address;
	char *port;
	char *ipv6;
	char *ssl;
	char *password;
	char *nick1;
	char *nick2;
	char *nick3;
	char *username;
	char *realname;
	char *hostname;
	char *command;
	char *autojoin;
};

/* globals */
struct config_pref_t config_pref;

/* rc file types */
void config_load(char *rcfile);

/* section types */
struct config_server_t *config_parse_server(FILE *in);
struct config_plugin_pref_t *config_parse_plugin(FILE *in);

/* entry types */
void config_read_key(FILE *in);
list_t *config_read_list(FILE *in);
char *config_read_string(FILE *in);
char *config_read_value(FILE *in);

#endif /* !PLUGINS_H_ */
