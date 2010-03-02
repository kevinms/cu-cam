#include <stdio.h>
#include <string.h>
#include "config.h"
#include "utils.h"

#define BSIZE 128

char key[BSIZE];
char value[BSIZE];

struct config_pref_t config_pref;

void
config_load(char *rcfile)
{
	char buf[BSIZE];

	FILE *in;
	in = fopen(rcfile,"r");
	if(in == NULL) {
		fprintf(stderr,"Error opening file: '%s'\n",rcfile);
		return;
	}

	struct config_client_pref_t *p_pref;
	struct config_server_pref_t *s_pref;

	s_pref = (struct config_server_pref_t *)malloc(sizeof(*s_pref));
	s_pref->list = list_init();

	/* go through all the sections in the config file */
	fgets(buf,BSIZE,in); buf[strlen(buf)-1]='\0';
	while(!feof(in)) {
		/* make sure it is really a section header */
		if(buf[0] == '[' || buf[strlen(buf)] == ']') {
			if(!strcmp(buf,"[server]"))
				list_add(s_pref->list,config_parse_server(in));
			else if(!strcmp(buf,"[client]"))
				p_pref = config_parse_client(in);
		}

		fgets(buf,BSIZE,in); buf[strlen(buf)-1]='\0';
	}

	config_pref.clients = p_pref;
	config_pref.servers = s_pref;
}

struct config_server_t *
config_parse_server(FILE *in)
{
	struct config_server_t *s;
	s = (struct config_server_t *)malloc(sizeof(*s));

	config_read_key(in);
	while(strlen(key)!=0 && !feof(in)) {
		if(!strcmp(key,"server_port_TCP"))
			s->port_TCP = config_read_value(in);
		else if(!strcmp(key,"server_port_UDP"))
			s->port_UDP = config_read_value(in);
		else if(!strcmp(key,"server_ipv6"))
			s->ipv6 = config_read_string(in);
		else if(!strcmp(key,"server_tls"))
			s->tls = config_read_string(in);
		config_read_key(in);
	}

	return s;
}

struct config_client_pref_t *
config_parse_client(FILE *in)
{
	struct config_client_pref_t *p;
	p = (struct config_client_pref_t *)malloc(sizeof(*p));

	config_read_key(in);
	while(strlen(key)!=0 && !feof(in)) {
		if(!strcmp(key,"server_list"))
			p->list = config_read_list(in);
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

	if(strlen(key)) {
		fgetc(in);fgetc(in);
	}
}

list_t *
config_read_list(FILE *in)
{
	list_t *list = list_init();

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
