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

	// Check for errors in the packet status
	if(inBuf[1] == STAT_BAD_PERM) {
		size = 0;
		buf[0] = CMD_PUT;
		buf[1] = STAT_BAD_PERM;
		size += 2;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else
	if(inBuf[1] == STAT_MALF_REQ) {
		size = 0;
		buf[0] = CMD_PUT;
		buf[1] = STAT_MALF_REQ;
		size += 2;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else
	if(inBuf[1] == STAT_UNK_STAT) {
		size = 0;
		buf[0] = CMD_PUT;
		buf[1] = STAT_UNK_STAT;
		size += 2;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else
	if(inBuf[1] == STAT_ERROR) {
		size = 0;
		buf[0] = CMD_PUT;
		buf[1] = STAT_ERROR;
		size += 2;
		net_send_tcp(sock, buf, size);
		exit(1);
	}else
	if(inBuf[1] == STAT_OK) {
		printf("Error: Everything is OK!\n");

		int fileSize = ntohl(*(int *)(inBuf+2));
		printf("Size Of File Is : %d\n", fileSize);
		//TODO: check that file System has enough space for file
		struct statvfs info;
		statvfs("/", &info);
		//TODO: check for other errors

		//clear size
		size = 0;

		//set flags
		buf[0] = CMD_PUT;
		buf[1] = STAT_OK;

		size += 2;

		//send all clear
		net_send_tcp(sock, buf, size);

		// RECEIVE FRAGMENTS AND PUT THEM IN A BUFFER
		net_recv_fragments_tcp(sock, &inBuf, fileSize);

		//TODO: FIX FILE LOCATION PLACEMENT STUFFS!!!

		FILE *fp = fopen(filename, "w+b");

		fwrite(inBuf, fileSize, 1, fp);

		fclose(fp);


		exit(1);
	}else {
		size = 0;
		buf[0] = CMD_PUT;
		buf[1] = STAT_ERROR;
		size += 2;
		net_send_tcp(sock, buf, size);
		exit(1);
	}
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

	// 
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

    int size = 0;
    buf[0] = CMD_PUT;
    size++;

    f = fopen(fileName, "rb");
	size++;
	if(f == NULL) {
            printf("Error: No such file.\n");
            return -1;
	} else {
		filesize = fsize(fileName);
		fprintf(stderr,"filesize: %d\n",filesize);
		//TODO: what if an int is not 4 bytes.... hmmmm.....
		*(int*)(buf+size) = htonl(filesize);

		size += 4;
		if(filesize > 0)
			buf[1] = STAT_OK;
		else
			buf[1] = STAT_BAD_SIZE;
	}
	net_send_tcp(n->sock, buf, size);



        inBuf = net_recv_tcp(n->sock);
	struct command_t * cmd = command_parse(inBuf);

	if(cmd->type != CMD_PUT);
		//TODO: error
	if(cmd->status != STAT_OK);
		//TODO: error

        memset(buf, 0, RCVBUFSIZE);
	size = 0;

	//fprintf(stderr,"type: %d, status: %d\n", cmd->type, cmd->status);

	buf[0] = CMD_GET;
	buf[1] = STAT_MF;
	size += 2;

        char *filedata;

	filedata = (char *)malloc(filesize);
	fprintf(stderr,"filesize: %d\n",filesize);
	fprintf(stderr,"fread: %d\n",(int)fread(filedata, 1, filesize, f));

	net_send_fragments_tcp(n->sock, filedata, filesize, 400);

    return 0;
}
