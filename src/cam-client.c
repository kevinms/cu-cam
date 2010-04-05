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
#include "config.h"

#define CMD_GET  1
#define CMD_PUT  2
#define CMD_STAT 3

void clientCNTCCode();

int
main(int argc, char **argv)
{
    int cmdType;

    //setup interupt
    signal (SIGINT, clientCNTCCode);

    //load config
    config_load("camclient.rc",CONFIG_CLIENT);
    
    if (argc < 2)    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Incorrect Usage, use man %s for details\n", argv[0]);
       exit(1);
    }

    //check for first flag
    if(strcmp(argv[1], "-p") == 0) {
        /*Start Put Code */
        fprintf(stdout, "putting...\n");
        cmdType = CMD_PUT;
        /*End Put Code */
    }else if(strcmp(argv[1], "-g") == 0) {
        /*Start Get Code */
        fprintf(stdout, "getting...\n");
        cmdType = CMD_GET;
        /*End Get Code */
    }else if(strcmp(argv[1], "-s") == 0) {
        /*Start Stat Code */
        fprintf(stdout, "gathering info...\n");
        cmdType = CMD_STAT;

        if(client == NULL) {
            fprintf(stderr, "Bad Config Load");
            exit(1);
        }

     	struct link_t *templink;
        struct list_t *sockList = list_init();

	templink = client->group->head;
	while(templink != NULL && (strcmp(((struct config_group_t *)(templink->item))->name, argv[2]) != 0))
		templink = templink->next;

        if(templink == NULL){
            fprintf(stderr, "%s not a group in config file\n", argv[2]);
            //TODO: check to see if its a real IP or Server
            list_add(sockList, net_create_tcp_socket(argv[2], client->port));

        } else {
            int i = 0;
            int len = ((struct config_group_t *)(templink->item))->servers->len;
            struct link_t *curr = templink;
            
            for(i = 0; i < len; i++) {
                list_add(sockList, net_create_tcp_socket((char *)(curr->item), client->port));
                curr= curr->next;
            }
           
        }

        /*End Stat Code */
    }else {
       fprintf(stderr, "Incorrect Usage, use man %s for details\n", argv[0]);
       exit(1);
    }








	
	

	return 0;
}

void clientCNTCCode() {
	printf("\nEmergency Quit Initiated...\n");
        exit(1);
}

#if 0
//HW3 REFERENCE
int main(int argc, char *argv[]) {

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]); /* Use given port, if any */
    initSendRate = atoi(argv[3]);
    sendRate = initSendRate;
    rateInc = atof(argv[4]);
    interval = atof(argv[5]);
    msgSize = atoi(argv[6]);
    dbgTime = atof(argv[7]);
    alarmData = dbgTime;
    stage = 1;


            /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
    servAddr.sin_family      = AF_INET;             /* Internet address family */
    servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    servAddr.sin_port        = htons(echoServPort); /* Server port */

    /* If user gave a dotted decimal address, we need to resolve it  */
    if (servAddr.sin_addr.s_addr == -1) {
        thehost = gethostbyname(servIP);
            servAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
    }

            /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithError("connect() failed");
    if(dbgTime !=0)
    alarm(dbgTime);
    for(;;) {
        if((((double)getTime() - (double)startTime) / (double)interval) > (double)stage){
            stage++;
            sendRate += sendRate * rateInc;
        }

        timeStamp = getTime();
        char msgData[msgSize];
        memset(msgData, 0, sizeof(msgData));
        len = msgSize+16;
        char header[16];

        *(unsigned int *)header = htonl(len);

        *(unsigned int *)(header + 4) = htonl(seqNum);
        *(double *)(header + 8) = htonl(seqNum);
        char data[len];
        for(i = 0; i < 16; i++)
            data[i] = header[i];
        for(i = 16; i < len; i ++)
            data[i] = msgData[i-16];
        buffer = data;



        /* Send the string to the server */
        if (send(sock, buffer, len, 0) != len)
            DieWithError("send() sent a different number of bytes than expected");
        numberMessagesSent++;

            /* Receive up to the buffer size bytes from the sender */
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE, 0)) <= 0)
                DieWithError("recv() failed or connection closed prematurely");

        totalBytesReceived += 16;
        numberMessagesReceived++;


        if(ntohl(*(unsigned int *)echoBuffer) != 16) {
            printf("Wrong Akk format\n");
            printf("Msg Len: %d\n", ntohl(*(unsigned int *)echoBuffer));
        }

        if(seqNum != ntohl(*(unsigned int *)(echoBuffer+ 4))) {
            //printf(" Wrong Sequence Number %d vs. %d\n",  seqNum, assembled);
            printf("Akk num: %d\n", ntohl(*(unsigned int *)(echoBuffer+4)));

        }

        sampleRTT = getTime() - timeStamp;
        
        if(sampleRTT < minRTT) {
            minRTT = sampleRTT;
        }
        if(seqNum == 0){
            avgRTT = sampleRTT;
        }
        avgRTT = (1-0.02)*avgRTT + 0.02*sampleRTT;
                //uncomment to see actual rate data
                //printf("sendRate : %d\n", sendRate);
        if(avgRTT > (4 * minRTT)) {
            sendRate = sendRate / 2;
            avgRTT = sampleRTT;
        }

        seqNum++;
        avgR = avgRTT;
        totalTime = getTime() - startTime;
        wait(((double)len/(double)sendRate));
    }
}
#endif
