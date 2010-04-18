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

}


int
put_request(struct net_t *n, struct list_t *list, char *fileName, char *saveLoc) {

    printf("Rocking the put\n");

    return 0;
}

