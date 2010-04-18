#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "get.h"
#include "command.h"
#include "utils.h"


void
get_handle(int sock, struct command_t *cmd)
{
	//cmd->
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

        inBuf = net_recv_tcp(n->sock);

        //TODO: FIX FILE LOCATION PLACEMENT STUFFS!!!
        
        FILE *fp = fopen(fileFromPath(fileName), "ab");

        fwrite(inBuf + 2, fileSize, 1, fp);

        fclose(fp);


        exit(1);
    }else {
        printf("Error: An unknown error has occured, now quitting\n");
        exit(1);
    }


    

    
    return 0;
}
