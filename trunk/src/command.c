#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"
#include "utils.h"

/* 
 * Parse the command string
 * 
 * Returns: NULL on failure
 *          struct command_t * on success
 */

struct command_t *
command_parse(char *buf)
{
	int rv;
	int size;
	char *par;
	char *tok;
	struct command_t *cmd;

	/* Get command type */
	size = strcspn(buf,"!");
	if(!size) {
		fprintf(stderr,"Error: No command type: '%s'\n",buf);
		return NULL;
	}
	if(strlen(buf) == size)
		return NULL;
	tok = strtok(buf,"!");

	/* malloc space for a new command */
	cmd = (struct command_t *)malloc(sizeof(*cmd));
	if(!cmd) {
		fprintf(stderr,"Error: malloc() failed for new struct command_t\n");
		return NULL;
	}
	cmd->users = NULL;
	cmd->files = NULL;

	/* 
	 * Determine and store what type of command
	 * Then, continue parsing the parameters
	 */
	par = tok + strlen(tok)+1;
	if(!strcmp(tok,"GET")) {
		cmd->type = CMD_GET;
		rv = command_parse_get(cmd,par);
	}
	else if(!strcmp(tok,"PUT")) {
		cmd->type = CMD_PUT;
		rv = command_parse_put(cmd,par);
	}
	else if(!strcmp(tok,"STAT")) {
		cmd->type = CMD_STAT;
		rv = command_parse_stat(cmd,par);
	}
	else {
		fprintf(stderr,"Error: Unknown command type '%s'\n",tok);
		free(cmd);
		return NULL;
	}

	if(!rv)
		return cmd;

	command_free(cmd);
	return NULL;
}

/*
 * Parse the parameters of a get command
 * 
 * Returns: -1 on failure
 *           0 on sucess
 */
int
command_parse_get(struct command_t *cmd, char *par)
{
	char *tok;

	/* Get the username */
	tok = command_parse_list(&cmd->users,par);
	if(!cmd->users || cmd->users->len != 1) {
		fprintf(stderr,"Error: No username provided or more than one\n");
		return -1;
	}

	/* Get what files/folders are wanted */
	if(!tok) {
		fprintf(stderr,"Error: Did not provide a file|folder to get\n");
		return -1;
	}
	tok = command_parse_list(&cmd->files,tok);
	if(!cmd->files) {
		fprintf(stderr,"Error: Did not provide a file|folder to get\n");
		return -1;
	}

	return 0;
}

/*
 * Parse the parameters of a put command
 * 
 * Returns: -1 on failure
 *           0 on sucess
 */
int
command_parse_put(struct command_t *cmd, char *par)
{
	char *tok;

	/* Get what users you want to send to */
	tok = command_parse_list(&cmd->users,par);
	if(!cmd->users) {
		fprintf(stderr,"Error: Did not provide user to send to\n");
		return -1;
	}

	/* Get what files/folders are wanted */
	if(!tok) {
		fprintf(stderr,"Error: Did not provide a file|folder to put\n");
		return -1;
	}
	tok = command_parse_list(&cmd->files,tok);
	if(!cmd->files) {
		fprintf(stderr,"Error: Did not provide a file|folder to put\n");
		return -1;
	}

	return 0;
}

/*
 * Parse the parameters of a stat command
 * 
 * Returns: -1 on failure
 *           0 on sucess
 */
int
command_parse_stat(struct command_t *cmd, char *par)
{
	char *tok;

	/* Get what users you want to get stats from*/
	tok = command_parse_list(&cmd->users,par);
	if(!cmd->users) {
		fprintf(stderr,"Error: Did not provide user to send to\n");
		return -1;
	}

	return 0;
}

char *
command_parse_list(struct list_t **list, char *buf)
{
	char *remainder = NULL;
	char *tok;
	int tmp;

	/* Find the place in buf that the list ends */
	if(!strlen(buf)) {
		fprintf(stderr,"Error: Could not parse command\n");
		return NULL;
	}
	tok = strpbrk(buf,":");
	if(tok) {
		remainder = tok+1;
		*tok = '\0';
	}

	if(!strlen(buf)) {
		fprintf(stderr,"Error: Could not parse list\n");
		return NULL;
	}

	/* Find and store all the elements for the list */
	*list = list_init();
	tok = buf;
	tmp = strcspn(tok,",");
	while(tmp!=strlen(tok)) {
		*(tok+tmp) = '\0';
		list_add(*list,p_strcpy(tok));
		tok = tok+tmp+1;
		tmp = strcspn(tok,",");
	}
	list_add(*list,p_strcpy(tok));

	return remainder;
}

void
command_free(struct command_t *cmd)
{
	if(cmd) {
		list_del(cmd->users);
		list_del(cmd->files);
		free(cmd);
		cmd = NULL;
	}
}
