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

    char buf[RCVBUFSIZE];
    int size = 0;
    char *tmp;
    //char *tok;
    FILE *f;
    int filesize;
    //struct list_t *username = ;
    char *filename;
    char *rbuf;
    char *filedata;



    buf[0] = CMD_PUT;
    size++;
    tmp = cmd->buf;


}


int
put_request(struct net_t *n, struct list_t *userName, char *fileName, char *saveLoc) {

    printf("Rocking the put\n");

    char *inBuf;
    char buf[256];
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

    strncpy(buf + dataSize, fileName, strlen(fileName));
    dataSize += (strlen(fileName));

    net_send_tcp(n->sock, buf, dataSize);

    return 0;
}

