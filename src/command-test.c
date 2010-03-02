#include <stdio.h>

#include "command.h"

void command_test_print(struct command_t *cmd);
void command_test_print_list(struct list_t *list);

void command_test_cmd(char buf[]);
void command_test_get(char buf[]);
void command_test_put(char buf[]);
void command_test_stat(char buf[]);

int main(int argc, char **argv)
{
	char cmd1[] = "GOAT!bob:";
	char cmd2[] = "!:";
	char cmd3[] = "!";
	char cmd4[] = ":skunk.c";
	char cmd5[] = "";
	char cmd6[] = "GET";

	char get1[] = "GET!bob:fred.c";
	char get2[] = "GET!:fred.c";
	char get3[] = "GET!bob:";
	char get4[] = "GET!bob:fred.c,bob.c";
	char get5[] = "GET!bob,george:fred.c";
	char get6[] = "GET!bob,sally:fred.c,bob.c";
	char get7[] = "GET!:";
	char get8[] = "GET!bob";
	char get9[] = "GET!";

	char put1[] = "PUT!bob:fred.c";
	char put2[] = "PUT!:fred.c";
	char put3[] = "PUT!bob:";
	char put4[] = "PUT!bob:fred.c,bob.c";
	char put5[] = "PUT!bob,george:fred.c";
	char put6[] = "PUT!bob,sally:fred.c,bob.c";
	char put7[] = "PUT!:";
	char put8[] = "PUT!bob";
	char put9[] = "PUT!";

	char stat1[] = "STAT!bob:LinuxVM1";
	char stat2[] = "STAT!:LinuxVM1";
	char stat3[] = "STAT!bob:";
	char stat4[] = "STAT!bob:LinuxVM1,LinuxVM2";
	char stat5[] = "STAT!bob,george:LinuxVM2";
	char stat6[] = "STAT!bob,sally:LinuxVM1,LinuxVM2";
	char stat7[] = "STAT!:";
	char stat8[] = "STAT!bob";
	char stat9[] = "STAT!";

	fprintf(stderr,"================================================================================\n");
	fprintf(stderr,"Testing Commands\n");
	command_test_cmd(cmd1);
	command_test_cmd(cmd2);
	command_test_cmd(cmd3);
	command_test_cmd(cmd4);
	command_test_cmd(cmd5);
	command_test_cmd(cmd6);

	fprintf(stderr,"================================================================================\n");
	fprintf(stderr,"Testing GET\n");
	command_test_get(get1);
	command_test_get(get2);
	command_test_get(get3);
	command_test_get(get4);
	command_test_get(get5);
	command_test_get(get6);
	command_test_get(get7);
	command_test_get(get8);
	command_test_get(get9);

	fprintf(stderr,"================================================================================\n");
	fprintf(stderr,"Testing PUT\n");
	command_test_put(put1);
	command_test_put(put2);
	command_test_put(put3);
	command_test_put(put4);
	command_test_put(put5);
	command_test_put(put6);
	command_test_put(put7);
	command_test_put(put8);
	command_test_put(put9);

	fprintf(stderr,"================================================================================\n");
	fprintf(stderr,"Testing STAT\n");
	command_test_stat(stat1);
	command_test_stat(stat2);
	command_test_stat(stat3);
	command_test_stat(stat4);
	command_test_stat(stat5);
	command_test_stat(stat6);
	command_test_stat(stat7);
	command_test_stat(stat8);
	command_test_stat(stat9);
	fprintf(stderr,"================================================================================\n");

	return 0;
}

void
command_test_cmd(char buf[])
{
	struct command_t *cmd;

	fprintf(stderr,"--------------------------------------------------------------------------------\n");
	cmd = command_parse(buf);
	if(!cmd)
		fprintf(stderr,"Error: Bad command could not parse\n");
	command_test_print(cmd);

	command_free(cmd);
}

void
command_test_get(char buf[])
{
	struct command_t *cmd;

	fprintf(stderr,"--------------------------------------------------------------------------------\n");
	cmd = command_parse(buf);
	if(!cmd)
		fprintf(stderr,"Error: Bad command could not parse\n");
	command_test_print(cmd);

	command_free(cmd);
}

void
command_test_put(char buf[])
{
	struct command_t *cmd;

	fprintf(stderr,"--------------------------------------------------------------------------------\n");
	cmd = command_parse(buf);
	if(!cmd)
		fprintf(stderr,"Error: Bad command could not parse\n");
	command_test_print(cmd);

	command_free(cmd);
}

void
command_test_stat(char buf[])
{
	struct command_t *cmd = NULL;

	fprintf(stderr,"--------------------------------------------------------------------------------\n");
	cmd = command_parse(buf);
	if(!cmd)
		fprintf(stderr,"Error: Bad command could not parse\n");
	command_test_print(cmd);

	command_free(cmd);
}

void
command_test_print(struct command_t *cmd)
{
	if(cmd) {
		fprintf(stderr,"Command type: %d\n",cmd->type);

		command_test_print_list(cmd->users);
		command_test_print_list(cmd->files);
	}
}

void
command_test_print_list(struct list_t *list)
{
	int i = 0;

	if(list) {
		fprintf(stderr,"List: ");
		list_set_list_to_iterate(list);
		while(list_peek_link()!=NULL) {
			if(i!=0)
				fprintf(stderr,", ");
			fprintf(stderr,"'%s'",(char *)list_peek_item());
			list_next_link();
			i++;
		}
		fprintf(stderr,"\n");
	}
}
