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
#include "net.h"

int
main(int argc, char **argv)
{
	struct net_t *
	//TODO:
	//argument checking

	
	

	return 0;
}

#if 0


double totalAverageThroughput = 0;
double totalAverageRTT = 0;
double avgR=0;
double totalTime = 0;
double sTime = 0;
long totalBytesSent = 0;
long totalBytesReceived = 0;
long numberMessagesSent = 0;
long numberMessagesReceived = 0;
int alarmData = 0;


void wait (double sec) {
  clock_t waitVal;
  waitVal = clock() + sec*CLOCKS_PER_SEC ;
  while (clock() < waitVal);
}

int main(int argc, char *argv[]) {
    double timeStamp;
    double startTime = getTime();
    sTime = startTime;
    int sock;                        /* Socket descriptor */
    struct sockaddr_in servAddr; /* Echo server address */
    struct hostent *thehost;         /* Hostent from gethostbyname() */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *buffer;                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int msgSize;      /* Length of Data*/
    unsigned int seqNum = 0;
    int bytesRcvd;   /* Bytes read in single recv() and total bytes read */

    unsigned long initSendRate, sendRate;
    double minRTT, sampleRTT, avgRTT;
    int len, i, stage;
    float rateInc, interval, dbgTime;
    struct sigaction alarmAction;
    alarmAction.sa_handler = catchAlarm;
    alarmAction.sa_flags = 0;
    if (sigfillset(&alarmAction.sa_mask) < 0)
        DieWithError("sigfillset() failed");
    if (sigaction(SIGALRM, &alarmAction, 0) < 0)
	DieWithError("sigaction() failed for SIGALRM");



    signal (SIGINT, clientCNTCCode);

    if (argc != 8)    /* Test for correct number of arguments */
    {
       //                           localhost      5000             100000                  0.2                 0.100                 10000                   1
       fprintf(stderr, "Usage: %s <Server IP> <Server Port> <Starting Send Rate(bps)> <Rate Increase> <Interval Period(sec)> <Message Size(bytes)>> <Debug Time(sec)>\n", argv[0]);
       exit(1);
    }

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
    //Never Reached! ((once its working that is...))
    close(sock);
    exit(0);
}
void quit_and_print() {
        totalAverageThroughput = totalBytesReceived / (double)totalTime;
        totalAverageRTT = avgR / (double)totalTime;
        printf("totalAverageThroughput %.1lf\ntotalAverageRTT = %.9lf\ntotalTime = %.1lf\ntotalBytesSent = %.1ld\ntotalBytesReceived = %.1ld\nnumberMessagesSent = %.1ld\nnumberMessagesReceived = %.1ld\n",
        totalAverageThroughput, totalAverageRTT, totalTime, totalBytesSent, totalBytesReceived, numberMessagesSent, numberMessagesReceived);
	exit(0);
}

void clientCNTCCode() {
	printf("TCPClient:  CNT-C Interrupt,  exiting....\n");
        alarm(0);
	quit_and_print();
}
void   catchAlarm(int ignored){
    alarm(0);
    totalTime = getTime() - sTime;
    totalAverageThroughput = totalBytesReceived / (double)totalTime;
    totalAverageRTT = avgR / totalTime;

    printf("Debug Data:\n");
    printf("Average Throughput: %.1lf\n",totalAverageThroughput);
    printf("Average RTT: %.9lf\n",avgR);
        alarm(alarmData);
    return;

}

#endif
