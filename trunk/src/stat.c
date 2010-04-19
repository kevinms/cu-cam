#include "stat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*Error types
STAT_MALF_REQ,
STAT_NOS_USER, 
STAT_UNK_STAT, 
STAT_ERROR
*/

/*    Assumptions:
	Server will go to stat_handle if the packet is 
		detected as non-TCP (UDP)
	Client will go to stat_request if the packet is 
		detected as non-TCP (UDP)
*/



/*    Global portion

	//#define ST_WHO  0  // Show who is logged on
	//#define ST_PROC 1  // Show process a user has running
	//#define ST_LS   2  // List the contents of a server

void packet_parse(){
	int task = -1;
	int 
}


*/


#define BUFLEN 512
#define NPACK 10


/*
int main(){
	stat_handle();
	return 0;
}
*/

//    Client portion
void stat_request(struct net_t *n, struct list_t *userName, char flag)
{
	//create packet
	char *inBuf;
    char buf[256];
    int dataSize = 0;

    buf[0] = CMD_STAT;
    buf[1] = STAT_OK;
	buf[2] = flag;	

    dataSize += 3;

	if(flag==ST_PROC){
		strncpy(buf + dataSize, userName->head->item, strlen(userName->head->item));
    	dataSize += (strlen(userName->head->item));
	}

	//send request packet to server
	net_send_tcp(n->sock, buf, dataSize);

	inBuf = net_recv_tcp(n->sock);
	fprintf(stderr, "inbuf[0]: %d, inbuf[1]: %d\n", inBuf[0],inBuf[1]);
	//handle errors
	if(inBuf[0] != CMD_STAT) {
        printf("Error: Recieved unexpected packet type\n");
        printf("Error Code: %d %d\n", inBuf[0],inBuf[1]);
        exit(1);
    }
	
	if(inBuf[1] == STAT_MALF_REQ) {
        printf("Error: Malformed request, now quitting\n");
        exit(1);
    }else
    if(inBuf[1] == STAT_NOS_USER) {
        printf("Error: No such user, now quitting\n");
        exit(1);
    }
    else
    if(inBuf[1] == STAT_UNK_STAT) {
        printf("Error: Unexpextes statistic return, now quitting\n");
        exit(1);
    }
    else
    if(inBuf[1] == STAT_ERROR) {
        printf("Error: An unknown error has occured, now quitting\n");
        exit(1);
    }

	dataSize = *(int *)(inBuf+2);
	dataSize = ntohl(dataSize);

	free(inBuf);
	net_recv_fragments_tcp(n->sock, &inBuf, dataSize);
	
	fprintf(stderr,"yoouuuurrrrr boooyy %s\n",inBuf);
}

//    Server portion
char *runCommand_getResults(char *command, int sock, struct command_t *cmd)
{
	int dataSize = 0;
	char buf[2+sizeof(int)];
	char *buf2;
	//running command on server
	system( command );

	//Storing results...
	int count = 0;
	FILE* tempFile;

	tempFile = fopen("STAT_temp.temp", "r" );
	fseek(tempFile,0,SEEK_END);
	count = ftell(tempFile);
	char *result;
	fseek(tempFile,0,SEEK_SET);
	
	result = (char*)calloc(count, sizeof(char));	

	if(result == NULL)
    	printf("ERROR MEMORY");
    
    fread(result, sizeof(char), count, tempFile);
	
	
	/*
	while( !feof( tempFile ) && tempFile )
	{
	ch = fgetc(tempFile);
	        if (ch == EOF)
	            break;
	        result[count] = 
	        ++count;
	}
	*/

	fclose( tempFile ); 
	
	buf[0] = CMD_STAT;
	buf[1] = STAT_OK;
	dataSize += 2;
	
	*(int *)(buf+2) = count;
	dataSize += sizeof(int);
	printf("buf[0]: %d, buf[1]: %d, size: %d\n",buf[0],buf[1],(buf+2));
	net_send_tcp(sock, buf, dataSize);
	
	buf2 = net_recv_tcp(sock);
	if(buf2[1] != STAT_OK)
		fprintf(stderr,"Bad status number\n");

	net_send_fragments_tcp(sock, result, count, 400);
	
	//printf("The file contains this\n\n%s", result);

	
	//result = (char *)malloc(strlen(result) + count);

	
	/*
	char line[100];
	FILE *tempFile2;
	  if((tempFile2 = fopen("STAT_temp.temp", "r")) == NULL) {
	    printf("Error Opening File.\n");
	    exit(1);
	  }
	  	
	while( fgets(line, sizeof(line), tempFile2) != NULL ) {
		
	    strcat(result,line);
	  }
	 
	fclose(tempFile2);
	*/
	
	//Removing temp file
	remove("STAT_temp.temp");

	return result;
}

void stat_handle(int sock, struct command_t *cmd)
{
	//Parse the packet to find out username and flag
	char *tmp;
	char flag;
	char *userName;
	char *finalCommand;
	
	tmp = cmd->buf;
	flag = tmp[0];
	tmp++;

	fprintf(stderr,"type: %d, status: %d, flag: %s\n",cmd->type, cmd->status, flag);
	
	if(flag == ST_WHO) //show users logged on
	{
		//char *finalCommand = "finger -lp > STAT_temp.temp";
		finalCommand = "users > STAT_temp.temp";
	}else if(flag == ST_PROC) //show processes of user
	{
		userName = command_parse_string(&tmp);

		finalCommand = "";
		char *command = "ps -ef | grep '";
		char *endCommand = "' > STAT_temp.temp";
  			finalCommand = (char *)calloc(strlen(userName) + strlen(command) + 
			strlen(endCommand)+ strlen(finalCommand) + 1, sizeof(char));

		strcat(finalCommand,command);
		strcat(finalCommand,userName);
		strcat(finalCommand,endCommand);
	}else if(flag == ST_LS) //show directory
	{
	
		//TODO: get directory passed in from client
		char *directory = "/home/burnsh/";
		finalCommand = "";
		char *command = "ls ";
		char *endCommand = " > STAT_temp.temp";
  			finalCommand = (char *)calloc(strlen(userName) + strlen(command) + 
			strlen(endCommand)+ strlen(finalCommand) + 1, sizeof(char));

		strcat(finalCommand,command);
		strcat(finalCommand,directory);
		strcat(finalCommand,endCommand);
	} else {
		fprintf(stderr,"BAD FLAG\n");
	}
	
	runCommand_getResults(finalCommand, sock, cmd);		
	
	return;
}




