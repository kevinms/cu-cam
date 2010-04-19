#include "put.h"

#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <stdlib.h>

#include "get.h"
#include "utils.h"

void
put_handle(int sock, struct command_t *cmd) {
	printf("Dealing with pesky puts...\n");

	struct list_t *username;
	char *filename;
	char *tmp = cmd->buf;
	char *inBuf;
	char buf[RCVBUFSIZE];
	int size = 0;
	char *user = NULL;
	char *path = "/home/";
	char *fullpath = NULL;
	FILE *fp;

	// Setup packet header
	buf[0] = CMD_PUT;
	buf[1] = STAT_OK;
	size += 2;

	// Break apart username(s) and files
	username = command_parse_list(&tmp);

	// Make sure user(s) exist
	struct link_t *templink = username->head;
	while(templink != username->tail){
		user = (char *)templink->item;
		if(fcheck_for_user(user) < 0) {
			printf("Error: bad username: %s\n",user);
			buf[1] = STAT_NOS_USER;
			net_send_tcp(sock, buf, size);
			close(sock);
			exit(0);
		}
		templink = templink->next;
	}

	// Get file name
	filename = templink->item;

	// Send STAT_OK packet to the client and get ready to recv
	net_send_tcp(sock, buf, size);

	// Recieve reply from server
	inBuf = net_recv_tcp(sock);

	// Make sure it is a CMD_PUT type
	if(inBuf[0] != CMD_PUT) {
		printf("Error: Recieved unexpected packet type\n");
		printf("Error Code: %d %d\n", inBuf[0],inBuf[1]);
		exit(1);
	}

	// Setup packet header
	size = 0;
	buf[0] = CMD_PUT;
	buf[1] = STAT_OK;
	size += 2;

	// Check for errors in the packet status
	if(inBuf[1] == STAT_BAD_PERM) {
		fprintf(stderr,"Error: bad permissions\n");
		buf[1] = STAT_BAD_PERM;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else if(inBuf[1] == STAT_MALF_REQ) {
		fprintf(stderr,"Error: Malformed Request\n");
		buf[1] = STAT_MALF_REQ;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else if(inBuf[1] == STAT_UNK_STAT) {
		fprintf(stderr,"Error: Unkown stat type\n");
		buf[1] = STAT_UNK_STAT;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else if(inBuf[1] == STAT_ERROR) {
		fprintf(stderr,"Error: General error\n");
		buf[1] = STAT_ERROR;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else if(inBuf[1] != STAT_OK) {
		fprintf(stderr,"Error: Undefined status type\n");
		buf[1] = STAT_ERROR;
		net_send_tcp(sock, buf, size);
		exit(1);
	}

	// Get filesize from packet
	int fileSize = ntohl(*(int *)(inBuf+2));
	printf("Size Of File Is : %d\n", fileSize);

	// Send STAT_OK packet
	net_send_tcp(sock, buf, size);

	// Recieve fragments and put them in a buffer
	net_recv_fragments_tcp(sock, &inBuf, fileSize);


	//TODO: FIX FILE LOCATION PLACEMENT STUFFS!!!
	templink = username->head;
	while(templink != username->tail){
		user = (char *)templink->item;
		

	// Set filename relative to the user's home folder
	fullpath = (char *)malloc(strlen(path)+strlen(user)+1+strlen(filename));
	memset(fullpath,0,strlen(path)+strlen(user)+1+strlen(filename));

	strcat(fullpath,path);
	strcat(fullpath,user);
	strcat(fullpath,"/");
	strcat(fullpath,filename);
	fprintf(stderr,"fullpath: '%s'\n",fullpath);


	fp = fopen(fullpath, "w+b");
	fwrite(inBuf, fileSize, 1, fp);
	fclose(fp);
	free(fullpath);

		
		templink = templink->next;
	}

	// Write all data to a file
	//FILE *fp = fopen(filename, "w+b");
	//fwrite(inBuf, fileSize, 1, fp);
	//fclose(fp);
	exit(1);
}


int
put_request(struct net_t *n, struct list_t *userName, char *fileName, char *saveLoc) 
{
	int filesize;
	FILE *f;
	char buf[RCVBUFSIZE];
	char *inBuf;
	int dataSize = 0;
	struct link_t *templink;
	struct command_t * cmd;
	char *filedata;

	// Setup packet header
	buf[0] = CMD_PUT;
	buf[1] = STAT_OK;
	dataSize += 2;

	// Add all usernames and filename to packet
	templink = userName->head;
	while(templink != NULL){
		strncpy(buf + dataSize, templink->item, strlen(templink->item));
		dataSize += (strlen(templink->item));

		if(templink != userName->tail) {
			buf[dataSize] = ',';
			dataSize ++;
		}
		templink = templink->next;
	}
	buf[dataSize] = ':';
	dataSize ++;

	strncpy(buf + dataSize, fileName, strlen(fileName));
	dataSize += (strlen(fileName));

	// Sending request packet to the server
	net_send_tcp(n->sock, buf, dataSize);

	// Recieve reply from server
	inBuf = net_recv_tcp(n->sock);

	// Make sure that packet type is CMD_PUT
	if(inBuf[0] != CMD_PUT) {
		printf("Error: Recieved unexpected packet type\n");
		return -1;
	}

	// Check for errors in the packet status
	if(inBuf[1] == STAT_BAD_PERM) {
		printf("Error: Improper permissions, now quitting\n");
		return -1;
	}else if(inBuf[1] == STAT_MALF_REQ) {
		printf("Error: Malformed request, now quitting\n");
		return -1;
	}else if(inBuf[1] == STAT_NOS_FILE) {
		printf("Error: No such file, now quitting\n");
		return -1;
	}else if(inBuf[1] == STAT_NOS_USER) {
		printf("Error: No such user, now quitting\n");
		return -1;
	}else if(inBuf[1] == STAT_UNK_STAT) {
		printf("Error: Unknown responce from server, now quitting\n");
		return -1;
	}else if(inBuf[1] == STAT_ERROR) {
		printf("Error: General error has occured, now quitting\n");
		return -1;
	}else if(inBuf[1] != STAT_OK) {
		printf("Error: An unknown error has occured %d, now quiting\n",inBuf[1]);
		return -1;
	}

	// Set the packet type
	int size = 0;
	buf[0] = CMD_PUT;
	buf[1] = STAT_OK;
	size+=2;

	// Make sure file exists
	if(fcheck_for_file(fileName) < 0) {
		printf("Error: bad filename: %s\n",fileName);
		return -1;
	}

	// Open file???? WHY????
	f = fopen(fileName, "rb");
	if(f == NULL) {
		printf("Error: Could not open file\n");
		return -1;
	}

	// Get file size and store it in packet
	filesize = fsize(fileName);
	*(int*)(buf+size) = htonl(filesize);
	size += 4;
	if(filesize <= 0) {
		fprintf(stderr,"Error: bad file size, %d", filesize);
		return -1;
	}

	// Send packet with STAT_OK and file size
	net_send_tcp(n->sock, buf, size);

	// Recieve a reply from the server
	inBuf = net_recv_tcp(n->sock);
	cmd = command_parse(inBuf);

	// Make sure it is a CMD_GET type
	if(cmd->type != CMD_PUT) {
		fprintf(stderr,"Error: Unexpected packet type\n");
		return -1;
	}

	// Make sure the client status is STAT_OK
	if(cmd->status != STAT_OK) {
		fprintf(stderr,"Error: Unexpected packet status, %d\n",cmd->status);
		return -1;
	}

	// Reset buf and size;
	memset(buf, 0, RCVBUFSIZE);
	size = 0;

	// Setup packet header
	buf[0] = CMD_GET;
	buf[1] = STAT_MF;
	size += 2;

	// Read in and store all the file data
	filedata = (char *)malloc(filesize);
	if(fread(filedata, 1, filesize, f) != filesize) {
		fprintf(stderr,"Error: Could not read all the data in file\n");
		return -1;
	}

	// Actually send the file!!
	//TODO: make the blocksize a config file option
	net_send_fragments_tcp(n->sock, filedata, filesize, 400);

	return 0;
}
