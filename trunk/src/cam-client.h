/* 
 * File:   cam-client.h
 *
 */

#ifndef _CAM_CLIENT_H
#define	_CAM_CLIENT_H

struct action_t {
	char function;
        int usesGroup;
        struct list_t *serverList;
        struct list_t *sockList;
        char *username;
        char *fileName;
        char *saveLoc;
	char *buf;
};

void clientCNTCCode();
struct action_t parseCmdArgs(int argc, char **argv);
int verifyGroup(char **argv);
void openAll(struct action_t job);
void closeAll(struct action_t job);

void getC(struct action_t job);
void putC(struct action_t job);
void statC(struct action_t job);

#endif	/* _CAM_CLIENT_H */

