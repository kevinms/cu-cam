/* 
 * Client program
 * 
 * Allow admin and students to send commands to all servers.  Depending
 * on what permissions the user has will determine what commands they 
 * can perform.
 * 
 */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netdb.h>      /* for getHostByName() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <signal.h>	
#include <sys/time.h>
#include <time.h>

#include "utils.h"
#include "command.h"
#include "net.h"
#include "get.h"
#include "put.h"
#include "stat.h"
#include "config.h"
#include "cam-client.h"

int
main(int argc, char **argv)
{
    //create new job
    struct action_t job;

    //setup interupt
    signal (SIGINT, clientCNTCCode);

    //load & check config
    config_load("camclient.rc",CONFIG_CLIENT);

    //Parse commandline arguments
    parseCmdArgs(argc, argv, &job);

    //open all sockets
    openAll(&job);

    //call proper command
    if(job.function == 'g'){
        getC(&job);
    } else
    if(job.function == 'p'){
        putC(&job);
    } else
    if(job.function == 's'){
        statC(&job);
    }

    //close all sockets
    closeAll(&job);

    //done
    return 0;
}

void
clientCNTCCode() {
    //TODO: stuff goes here
	printf("\nEmergency Quit Initiated...\n");
        exit(1);
}

void
getC(struct action_t *job){

        struct net_t *netLink;

        netLink = (struct net_t *)job->sockList->head->item;

        printf("connecting\n");
	net_connect(netLink);

        if(get_request(netLink, job->username, job->fileName, job->saveLoc) == -1) {
            printf("General Get Error Message Goes Here");
            exit(1);
        }

        printf("Done\n");
}

void
putC(struct action_t *job){
//TODO: stuff goes here

    struct link_t *templink;
    struct link_t *templinkName;
    
    templink = job->sockList->head;
    templinkName = job->serverList->head;

    
    while(templink != NULL && templinkName != NULL){
        printf("connecting to %s...",(char*)templinkName->item);
        net_connect((struct net_t *)templink->item);
        printf("Connected\n");
        printf("Putting...");
        if(put_request((struct net_t *)templink->item, job->username, job->fileName, job->saveLoc) == -1) {
            printf("Failed To Put To Server %s\n",(char*)templinkName->item);
        } else {
            printf("Put finished\n");
        }
        templinkName = templinkName->next;
        templink = templink->next;
    }
    printf("Done\n");
}

void
statC(struct action_t *job){

    char *flag  = job->saveLoc;

    struct link_t *templink;
    struct link_t *templinkName;

    templink = job->sockList->head;
    templinkName = job->serverList->head;


    while(templink != NULL && templinkName != NULL){
        printf("connecting to %s...",(char*)templinkName->item);
        net_connect((struct net_t *)templink->item);
        printf("Connected\n");
        printf("Putting...");
        stat_request((struct net_t *)templink->item, job->username, flag);
        templinkName = templinkName->next;
        templink = templink->next;
    }

        printf("Done\n");
}

void
openAll(struct action_t *job){

        struct link_t *templink;

        templink = job->serverList->head;

	while(templink != NULL){
            if(job->function != 's'){
                //printf("connecting to %s via port %s\n", (char *)templink->item, client->port);
                
                list_add(job->sockList, net_create_tcp_socket(templink->item, client->port));
            } else {
                list_add(job->sockList, net_create_udp_socket(templink->item, client->port));
            }
            templink = templink->next;
        }
}

void
closeAll(struct action_t *job){
        struct link_t *templink;

        templink = job->serverList->head;


	while(templink != NULL){
            //TODO:create close socket routine
            templink = templink->next;
        }
}

void
parseCmdArgs(int argc, char **argv,struct action_t *currentAction)  {

    //check first flag
    if(strcmp(argv[1], "-g") == 0 && argc == 6) {
        currentAction->function = 'g';
        currentAction->fileName = argv[4];
        currentAction->saveLoc = argv[5];
    }else if(strcmp(argv[1], "-p") == 0 && argc == 5) {
        currentAction->function = 'p';
        currentAction->fileName = argv[4];
        currentAction->saveLoc = NULL;
    }else if(strcmp(argv[1], "-s") == 0 && argc == 5) {
        currentAction->function = 's';
        currentAction->fileName = NULL;
        currentAction->saveLoc = argv[4];
    }else {
        fprintf(stderr, "Incorrect Usage, use man %s for details\n", argv[0]);
        exit(1);
    }

    currentAction->sockList = list_init();

    if(verifyGroup(argv, currentAction) == (-1)) {
        //Does not use group
        currentAction->usesGroup = -1;
    }else {
        //Uses group
        currentAction->usesGroup = 1;
    }

 
}


int
verifyGroup(char **argv, struct action_t *job) {
        struct link_t *templink;
        
        templink = client->group->head;

	while(templink != NULL && (strcmp(((struct config_group_t *)(templink->item))->name, argv[2]) != 0))
		templink = templink->next;

        if(templink == NULL){
            fprintf(stderr, "%s not a group in config file\n", argv[2]);
            job->serverList = list_init();
            job->username = list_init();
            list_add(job->serverList, argv[2]);        
            list_add(job->username, argv[3]);

            return -1;
        } else{
            job->serverList = ((struct config_group_t *)(templink->item))->servers;
            job->serverList = ((struct config_group_t *)(templink->item))->users;
            return 1;
        }
}