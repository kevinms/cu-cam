#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "utils.h"

#define BSIZE 128

char key[BSIZE];
char value[BSIZE];

struct config_server_t *server = NULL;
struct config_client_t *client = NULL;

void
config_load(char *rcfile, int type)
{
	FILE *in;
	in = fopen(rcfile,"r");
	if(in == NULL) {
		fprintf(stderr,"Error opening file: '%s'\n",rcfile);
		return;
	}

	if(type == CONFIG_SERVER) {
		server = config_parse_server(in);
	}
	else if(type == CONFIG_CLIENT) {
		client = config_parse_client(in);
		if(client == NULL) {
			fprintf(stderr, "Bad Config Load\n");
			exit(1);
		}
	}
}

struct config_server_t *
config_parse_server(FILE *in)
{
	char *tmp;
	struct config_server_t *s;
	s = (struct config_server_t *)malloc(sizeof(*s));

	config_read_key(in);
	while(strlen(key)!=0 && !feof(in)) {
		if(!strcmp(key,"server_port"))
			s->port = config_read_value(in);
		else if(!strcmp(key,"server_ipv6"))
			s->ipv6 = config_read_string(in);
		else if(!strcmp(key,"server_tls"))
			s->tls = config_read_string(in);
		else if(!strcmp(key,"server_deamon")) {
			tmp = config_read_value(in);
			s->deamon = atoi(tmp);
			free(tmp);
		}
		config_read_key(in);
	}

	return s;
}

struct config_client_t *
config_parse_client(FILE *in)
{
	char *tmp;
	struct config_client_t *p;
	p = (struct config_client_t *)malloc(sizeof(*p));
	p->group = list_init();

	config_read_key(in);
	while(!feof(in)) {
		if(!strcmp(key,"[group]"))
			list_add(p->group,config_parse_group(in));
		else if(!strcmp(key,"server_port"))
			p->port = config_read_value(in);
		else if(!strcmp(key,"client_stat_interval")) {
			tmp = config_read_value(in);
			p->stat_interval = atoi(tmp);
			free(tmp);
		}
		config_read_key(in);
	}

	return p;
}

struct config_group_t *
config_parse_group(FILE *in)
{
	struct config_group_t *p;
	p = (struct config_group_t *)malloc(sizeof(*p));

	config_read_key(in);
	while(strlen(key) && !feof(in)) {
		if(!strcmp(key,"group_name")) {
			p->name = config_read_string(in);
		}
		else if(!strcmp(key,"group_servers")) {
			p->servers = config_read_list(in);
		}
		else if(!strcmp(key,"group_users")) {
			p->users = config_read_list(in);
		}
		config_read_key(in);
	}

	return p;
}

void
config_read_key(FILE *in)
{
	int i = 0;
	key[i] = fgetc(in);
	while(i<BSIZE-1 && key[i]!=' ' && key[i]!='\n') {
		i++;
		key[i] = fgetc(in);
	}
	key[i] = '\0';

	if(strlen(key) && key[0] != '[' && key[strlen(key)-1] != ']') {
		fgetc(in);fgetc(in);
	}
}

struct list_t *
config_read_list(FILE *in)
{
	struct list_t *list = list_init();

	if(fgetc(in)!='{') {
		fprintf(stderr,"Error parsing value for key: '%s'\n",key);
		return NULL;
	}

	while(fgetc(in)!='\n');

	fscanf(in,"%s",value);
	while(strcmp(value,"}")) {
		list_add(list,p_strcpy(value));
		fscanf(in,"%s",value);
	}

	fgetc(in);

	return list;
}

char *
config_read_string(FILE *in)
{
	int i = 0;
	fgetc(in);

	value[i] = fgetc(in);
	while(i<BSIZE-1 && value[i]!='"') {
		i++;
		value[i] = fgetc(in);
	}
	value[i] = '\0';
	fgetc(in);
	return p_strcpy(value);
}

char *
config_read_value(FILE *in)
{
	int i = 0;

	value[i] = fgetc(in);
	while(i<BSIZE-1 && value[i]!='\n') {
		i++;
		value[i] = fgetc(in);
	}
	value[i] = '\0';
	return p_strcpy(value);
}
