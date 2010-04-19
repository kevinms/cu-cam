#include "stat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define BUFLEN 512
#define NPACK 10

void stat_request(struct net_t *n, struct list_t *userName, char flag)
{
	char *inBuf;
	char buf[256];
	int size = 0;
	int dataSize = 0;

	// Setup packet header
	buf[0] = CMD_STAT;
	buf[1] = STAT_OK;
	buf[2] = flag;
	dataSize += 3;

	// Add any extra information to the packet (username, filename, etc)
	if(flag==ST_PROC||flag==ST_LS){
		fprintf(stderr,"username: %s\n",(char *)userName->head->item);
		strncpy(buf + dataSize, (char *)userName->head->item, strlen((char *)userName->head->item));
		dataSize += (strlen((char *)userName->head->item));
	}

	// Send request packet to server
	net_send_tcp(n->sock, buf, dataSize);

	// Recieve reply from server
	inBuf = net_recv_tcp(n->sock);

	// Make sure it is a CMD_GET type
	if(inBuf[0] != CMD_STAT) {
		printf("Error: Recieved unexpected packet type\n");
		printf("Error Code: %d %d\n", inBuf[0],inBuf[1]);
		exit(1);
	}

	// Make sure the client status is STAT_OK
	if(inBuf[1] == STAT_MALF_REQ) {
		printf("Error: Malformed request, now quitting\n");
		exit(1);
	}else if(inBuf[1] == STAT_NOS_USER) {
		printf("Error: No such user, now quitting\n");
		exit(1);
	}
	else if(inBuf[1] == STAT_UNK_STAT) {
		printf("Error: Unexpextes statistic return, now quitting\n");
		exit(1);
	}
	else if(inBuf[1] == STAT_ERROR) {
		printf("Error: An unknown error has occured, now quitting\n");
		exit(1);
	}

	// Store the file size
	dataSize = *(int *)(inBuf+2);
	dataSize = ntohl(dataSize);

	free(inBuf);
	size = 0;
	buf[0] = CMD_STAT;
	buf[1] = STAT_OK;
	size += 2;

	net_send_tcp(n->sock,buf,size);

	net_recv_fragments_tcp(n->sock, &inBuf, dataSize);

	// Print out the recieved stats
	printf("================================================================================\n");
	printf("                        Data Recieved From The Server                           \n");
	printf("================================================================================\n");
	printf("%s\n",inBuf);
}

char *runCommand_getResults(char *command, int sock, struct command_t *cmd)
{
	int dataSize = 0;
	char buf[2+sizeof(int)];
	char *buf2;
	//running command on server
	printf("command: %s\n",command);
	system( command );

	//Storing results...
	int count = 0;
	FILE* tempFile;


/*
	fseek(tempFile,0,SEEK_END);
	count = ftell(tempFile);
*/
	count = fsize("STAT_temp.temp");
	fprintf(stderr,"count: %d\n", count);
	
	printf("bob2\n");
	char *result = NULL;
//	fseek(tempFile,0,SEEK_SET);
	printf("bob1\n");
	result = (char*)malloc(count);
	printf("bob4\n");
	if(result == NULL)
		printf("ERROR MEMORY");

	tempFile = fopen("STAT_temp.temp", "r");
	fread(result, 1, count, tempFile);

	fclose( tempFile ); 

	buf[0] = CMD_STAT;
	buf[1] = STAT_OK;
	dataSize += 2;
	
	*(int *)(buf+2) = htonl(count);
	dataSize += sizeof(int);
	printf("buf[0]: %d, buf[1]: %d, size: %d\n",buf[0],buf[1],*(int *)(buf+2));
	net_send_tcp(sock, buf, dataSize);
	
	buf2 = net_recv_tcp(sock);
	if(buf2[1] != STAT_OK)
		fprintf(stderr,"Bad status number\n");

	net_send_fragments_tcp(sock, result, count, 400);
	
	//printf("The file contains this\n\n%s", result);
	//result = (char *)malloc(strlen(result) + count);
	
	//Removing temp file
	remove("STAT_temp.temp");

	return result;
}

void stat_handle(int sock, struct command_t *cmd)
{
	printf("asdfasfasdfasfdasdf\n");
	//Parse the packet to find out username and flag
	char *tmp;
	char flag;
	char *userName;
	char *finalCommand;
	char *command;
	char *endCommand;

	tmp = cmd->buf;
	printf("cmd->buf: %d\n", cmd->buf[0]);
	//flag = 0;
	flag = tmp[0];
	tmp++;

	fprintf(stderr,"type: %d, status: %d, flag: %d\n",cmd->type, cmd->status, flag);
	
	if(flag == ST_WHO) //show users logged on
	{
		//char *finalCommand = "finger -lp > STAT_temp.temp";
		finalCommand = p_strcpy("w > STAT_temp.temp");
	}else if(flag == ST_PROC) //show processes of user
	{
		userName = command_parse_string(&tmp);

		command = p_strcpy("ps -ef | grep '");
		endCommand = p_strcpy("' > STAT_temp.temp");

		finalCommand = (char *)calloc(strlen(userName) + strlen(command) + 
		strlen(endCommand) + 1, sizeof(char));

		strcat(finalCommand,command);
		strcat(finalCommand,userName);
		strcat(finalCommand,endCommand);

	}else if(flag == ST_LS) //show directory
	{
		char *directory = command_parse_string(&tmp);
		fprintf(stderr,"directory: %s\n",directory);

//		finalCommand = p_strcpy("ls /home/halp/ > STAT_temp.temp");

		command = p_strcpy("ls ");
		endCommand = p_strcpy(" > STAT_temp.temp");

		fprintf(stderr,"malloc size: %d\n",strlen(command) + strlen(endCommand) + strlen(directory));
		finalCommand = (char *)malloc(strlen(command) + strlen(endCommand) + strlen(directory));
		memset(finalCommand, 0, strlen(command) + strlen(endCommand) + strlen(directory));
		fprintf(stderr,"bob\n");
		strcat(finalCommand,command);
		strcat(finalCommand,directory);
		strcat(finalCommand,endCommand);

		printf("finalCommand: '%s'\n",finalCommand);
	} else {
		fprintf(stderr,"BAD FLAG\n");
	}
	
	fprintf(stderr,"runCommand_getResults\n");
	runCommand_getResults(finalCommand, sock, cmd);
	
	return;
}
