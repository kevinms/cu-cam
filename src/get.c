#include <stdio.h>
#include "get.h"
#include "utils.h"
#include <string.h>
#include <sys/statvfs.h>

#include <stdlib.h>


void
get_handle(int sock, struct command_t *cmd)
{
	char buf[RCVBUFSIZE];
	int size = 0;
	char *tmp;
	//char *tok;
	FILE *f;
	int filesize;
	char *username;
	char *filename;
	char *rbuf;
	char *filedata;

	//void net_send_tcp(int sock, char *buf, int size);
	//char *c(int sock);

	buf[0] = CMD_GET;
	size++;
	tmp = cmd->buf;

/*******************************************************************************
  Sending file size or error
*******************************************************************************/
	//TODO: make sure user exists
	username = command_parse_string(&tmp);
	fprintf(stderr,"user: %s\n",username);
	//buf[1] = STAT_NOS_USER;

	//TODO: make sure file exists
	fprintf(stderr,"tmp: %s\n",tmp);
	filename = command_parse_string(&tmp);
	fprintf(stderr,"filename: %s\n",filename);

	f = fopen(filename, "rb");
	size++;
	if(f == NULL) {
		buf[1] = STAT_NOS_FILE;
	} else {
		filesize = fsize(filename);
		fprintf(stderr,"filesize: %d\n",filesize);
		//TODO: what if an int is not 4 bytes.... hmmmm.....
		*(int*)(buf+size) = htonl(filesize);
		
		fprintf(stderr,"filesize: %d\n",*(int*)(buf+size));
		size += 4;
		if(filesize > 0)
			buf[1] = STAT_OK;
		else
			buf[1] = STAT_BAD_SIZE;
	}
	net_send_tcp(sock, buf, size);
	command_free(cmd);

/*******************************************************************************
  Send file!!!
*******************************************************************************/
	rbuf = net_recv_tcp(sock);
	cmd = command_parse(rbuf);

	if(cmd->type != CMD_GET);
		//TODO: error

	if(cmd->status != STAT_OK);
		//TODO: error

	memset(buf, 0, RCVBUFSIZE);
	size = 0;

	fprintf(stderr,"type: %d, status: %d\n", cmd->type, cmd->status);

	buf[0] = CMD_GET;
	buf[1] = STAT_MF;
	size += 2;

	filedata = (char *)malloc(filesize);
	fprintf(stderr,"filesize: %d\n",filesize);
	fprintf(stderr,"fread: %d\n",fread(filedata, 1, filesize, f));

	net_send_fragments_tcp(sock, filedata, filesize, 512);
}


int
get_request(struct net_t *n, struct list_t *userName, char *fileName, char *saveLoc)
{
    char *inBuf;
    char buf[256];
    int dataSize = 0;

    buf[0] = CMD_GET;
    buf[1] = STAT_OK;

    dataSize += 2;

    strncpy(buf + dataSize, userName->head->item, strlen(userName->head->item));
    dataSize += (strlen(userName->head->item));

    buf[dataSize] = ':';
    dataSize ++;

    strncpy(buf + dataSize, fileName, strlen(fileName));
    dataSize += (strlen(fileName));

    net_send_tcp(n->sock, buf, dataSize);

    dataSize = 0;
    inBuf = net_recv_tcp(n->sock);
    if(inBuf[0] != CMD_GET) {
        printf("Error: Recieved unexpected packet type\n");
        printf("Error Code: %d %d\n", inBuf[0],inBuf[1]);
        exit(1);
    }

    if(inBuf[1] == STAT_BAD_PERM) {
        printf("Error: Improper permissions, now quitting\n");
        exit(1);
    }else
    if(inBuf[1] == STAT_MALF_REQ) {
        printf("Error: Malformed request, now quitting\n");
        exit(1);
    }else
    if(inBuf[1] == STAT_NOS_FILE) {
        printf("Error: No such file, now quitting\n");
        exit(1);
    }else
    if(inBuf[1] == STAT_NOS_USER) {
        printf("Error: No such user, now quitting\n");
        exit(1);
    }else
    if(inBuf[1] == STAT_UNK_STAT) {
        printf("Error: Unknown responce from server, now quitting\n");
        exit(1);
    }else
    if(inBuf[1] == STAT_ERROR) {
        printf("Error: An unknown error has occured, now quitting\n");
        exit(1);
    }else
    if(inBuf[1] == STAT_OK) {
        printf("Error: Everything is OK!\n");

        int fileSize = ntohl(*(int *)(inBuf+2));
        printf("Size Of File Is : %d\n", fileSize);
        //TODO: check that file System has enough space for file
        struct statvfs info;
        statvfs("/", &info);
        printf("%d\n",(info.f_bavail * info.f_bsize));
        if(info.f_bavail * info.f_bsize < fileSize) {
            printf("Error: Not enough free space\n");
            exit(1);
        }

        //TODO: check for other errors

        //clear buff
        memset(buf, 0, 256);
        dataSize = 0;

        //set flags
        buf[0] = CMD_GET;
        buf[1] = STAT_OK;
        
        dataSize += 2;

        //send all clear
        net_send_tcp(n->sock, buf, dataSize);

	// RECEIVE FRAGMENTS AND PUT THEM IN A BUFFER
        net_recv_fragments_tcp(n->sock, &inBuf, fileSize);

        //TODO: FIX FILE LOCATION PLACEMENT STUFFS!!!
        
        FILE *fp = fopen(saveLoc, "ab");

        fwrite(inBuf, fileSize, 1, fp);

        fclose(fp);


        exit(1);
    }else {
        printf("Error: An unknown error has occured, now quitting\n");
        exit(1);
    }


    

    
    return 0;
}
