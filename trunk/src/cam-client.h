/* 
 * File:   cam-client.h
 *
 */

#ifndef _CAM_CLIENT_H
#define	_CAM_CLIENT_H

struct action_t {
	char function;
        int usesGroup;
        struct list_t *serverList;// a list of servers
        struct list_t *sockList;  // a list of sockets
        struct list_t *username;  // a list of char *
        char *fileName;
        char *saveLoc;
};

void clientCNTCCode();
void parseCmdArgs(int argc, char **argv, struct action_t *job);
int verifyGroup(char **argv, struct action_t *job);
void openAll(struct action_t *job);
void closeAll(struct action_t *job);

void getC(struct action_t *job);
void putC(struct action_t *job);
void statC(struct action_t *job);

#endif	/* _CAM_CLIENT_H */

