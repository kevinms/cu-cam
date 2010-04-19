#include "put.h"
#include <stdio.h>
#include "get.h"
#include "utils.h"
#include <string.h>
#include <sys/statvfs.h>

#include <stdlib.h>

void
put_handle(int sock, struct command_t *cmd) {
    printf("Dealing with pesky puts...\n");

    char *inBuf;
    char buf[RCVBUFSIZE];
    int size = 0;
    //char *tok;
    
    struct list_t *username;
    char *filename;
    //char *rbuf;
    //char *filedata;
    char *tmp = cmd->buf;

    username = command_parse_list(&tmp);

//start debug print
    struct link_t *templink = username->head;
    while(templink != username->tail){
       fprintf(stderr,"user: %s\n", (char *)templink->item);
       templink = templink->next;
    }
//end debug print

    filename = templink->item;
    fprintf(stderr,"filename: '%s'\n",filename);

    //setup and send "all clear" signal
    //TODO: error checking to make sure everyting IS ok
    size = 0;
    buf[0] = CMD_PUT;
    buf[1] = STAT_OK;
    size += 2;
    net_send_tcp(sock, buf, size);

    inBuf = net_recv_tcp(sock);
    if(inBuf[0] != CMD_PUT) {
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
        printf("Error: An unknown error has occured, now quitting\n");
        exit(1);
    }
}


int
put_request(struct net_t *n, struct list_t *userName, char *fileName, char *saveLoc) {

    printf("Rocking the put\n");

    int filesize;
    FILE *f;
    char buf[RCVBUFSIZE];
    char *inBuf;
    int dataSize = 0;

    buf[0] = CMD_PUT;
    buf[1] = STAT_OK;

    dataSize += 2;

    struct link_t *templink;

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

    printf("filename : '%s' " , fileName);
    strncpy(buf + dataSize, fileName, strlen(fileName));
    dataSize += (strlen(fileName));

    net_send_tcp(n->sock, buf, dataSize);


    int size = 0;
    buf[0] = CMD_PUT;
    size++;

    f = fopen(fileName, "rb");
	size++;
	if(f == NULL) {
		buf[1] = STAT_NOS_FILE;
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

