#include "get.h"

#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <stdlib.h>

#include "utils.h"

void
get_handle(int sock, struct command_t *cmd)
{
	char buf[RCVBUFSIZE];
	int size = 0;
	char *tmp;
	FILE *f;
	int filesize;
	char *username;
	char *filename;
	char *rbuf;
	char *filedata;
	char *path = "/home/";
	char *fullpath = NULL;

	// Set the packet type
	buf[0] = CMD_GET;
	size++;
	tmp = cmd->buf;

	// Make sure user exists
	username = command_parse_string(&tmp);
	if(fcheck_for_user(username) < 0) {
		printf("Error: bad username: %s\n",username);
		buf[1] = STAT_NOS_USER;
		size++;
		net_send_tcp(sock, buf, size);
		close(sock);
		exit(0);
	}

	// Set filename relative to the user's home folder
	filename = command_parse_string(&tmp);
	fullpath = (char *)malloc(strlen(path)+strlen(username)+1+strlen(filename));
	memset(fullpath,0,strlen(path)+strlen(username)+1+strlen(filename));

	strcat(fullpath,path);
	strcat(fullpath,username);
	strcat(fullpath,"/");
	strcat(fullpath,filename);
	fprintf(stderr,"fullpath: '%s'\n",fullpath);

	// Make sure file exists
	if(fcheck_for_file(fullpath) < 0) {
		printf("Error: bad filename: %s\n",fullpath);
		buf[1] = STAT_NOS_FILE;
		size++;
		net_send_tcp(sock, buf, size);
		close(sock);
		exit(0);
	}

	// Set the status byte
	buf[1] = STAT_OK;
	size++;

	// Get the file size
	f = fopen(fullpath, "rb");
	filesize = fsize(fullpath);
	*(int*)(buf+size) = htonl(filesize);
	size += 4;
	if(filesize <= 0) {
		buf[1] = STAT_BAD_SIZE;
		net_send_tcp(sock, buf, size);
		close(sock);
		exit(0);
	}

	// Send reply with the filesize
	net_send_tcp(sock, buf, size);
	command_free(cmd);

	// Recieve ready packet from client
	rbuf = net_recv_tcp(sock);
	cmd = command_parse(rbuf);

	// Reset buf and size
	memset(buf, 0, RCVBUFSIZE);
	size = 0;

	// Setup packet header
	buf[0] = CMD_GET;
	buf[1] = STAT_MF;
	size += 2;

	// Make sure it is a CMD_GET type
	if(cmd->type != CMD_GET) {
		fprintf(stderr,"Error: Unexpected packet type\n");
		buf[1] = STAT_UNK_STAT;
		net_send_tcp(sock, buf, size);
		close(sock);
		exit(0);
	}

	// Make sure the client status is STAT_OK
	if(cmd->status != STAT_OK) {
		fprintf(stderr,"Error: Unexpected packet status\n");
		buf[1] = STAT_ERROR;
		net_send_tcp(sock, buf, size);
		close(sock);
		exit(0);
	}

	// Read in and store all the file data
	filedata = (char *)malloc(filesize);
	if(fread(filedata, 1, filesize, f) != filesize) {
		fprintf(stderr,"Error: Could not read all the data in file\n");
		buf[1] = STAT_ERROR;
		net_send_tcp(sock, buf, size);
		close(sock);
		exit(0);
	}

	// Actually send the file!!
	//TODO: make the blocksize a config file option
	net_send_fragments_tcp(sock, filedata, filesize, 400);

	exit(0);
}

int
get_request(struct net_t *n, struct list_t *userName, char *fileName, char *saveLoc)
{
	char *inBuf;
	char buf[256];
	int dataSize = 0;

	// Setup packet header
	buf[0] = CMD_GET;
	buf[1] = STAT_OK;
	dataSize += 2;

	// Append username to packet
	strncpy(buf + dataSize, userName->head->item, strlen(userName->head->item));
	dataSize += (strlen(userName->head->item));

	// Append token seperator to packet
	buf[dataSize] = ':';
	dataSize ++;

	// Append filename to packet
	strncpy(buf + dataSize, fileName, strlen(fileName));
	dataSize += (strlen(fileName));

	// Send get request to server
	net_send_tcp(n->sock, buf, dataSize);

	// Recieve reply from server
	inBuf = net_recv_tcp(n->sock);

	dataSize = 0;

	// Make sure that packet type is CMD_GET
	if(inBuf[0] != CMD_GET) {
		printf("Error: Recieved unexpected packet type: %d\n", inBuf[0]);
		exit(1);
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

	// If STAT_OK then get ready to recieve the file from the server
	// Get filesize from packet and make sure there is enough space for it
	int fileSize = ntohl(*(int *)(inBuf+2));

	//TODO: FIX THIS!!!check that file System has enough space for file
	struct statvfs info;
	statvfs("/", &info);
	printf("freespace remaining: %d\n",(int)(info.f_bavail * info.f_bsize));
	if(info.f_bavail * info.f_bsize < fileSize) {
		printf("Error: Not enough free space\n");
		exit(1);
	}

	// Reset buf and dataSize
	memset(buf, 0, 256);
	dataSize = 0;

	// Setup packet header
	buf[0] = CMD_GET;
	buf[1] = STAT_OK;
	dataSize += 2;

	// Send STAT_OK packet to server
	net_send_tcp(n->sock, buf, dataSize);

	// Recieve the entire file and store it in inBuf
	net_recv_fragments_tcp(n->sock, &inBuf, fileSize);

	// Write all data to a file
	FILE *fp = fopen(saveLoc, "w+b");
	fwrite(inBuf, fileSize, 1, fp);
	fclose(fp);
	
	return 0;
}
