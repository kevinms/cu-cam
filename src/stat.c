#include "stat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

/*Error types
STAT_MALF_REQ,
STAT_NOS_USER, 
STAT_UNK_STAT, 
STAT_ERROR
*/

/*    Assumptions:
	Server will go to stat_handle if the packet is 
		detected as non-TCP (UDP)
	Client will go to stat_request if the packet is 
		detected as non-TCP (UDP)
*/



/*    Global portion

	//#define ST_WHO  0  // Show who is logged on
	//#define ST_PROC 1  // Show process a user has running
	//#define ST_LS   2  // List the contents of a server

void packet_parse(){
	int task = -1;
	int 
}

*/

#if 0
int main(){
	stat_handle();
	return 0;
}
#endif

//    Client portion
void stat_request()
{
	//send request packet to server

	//handle reply packet


	//print data

}



//    Server portion

char *runCommand_getResults(char *command)
{

	//char *result;
	
	//running command on server
	int processList = system( command );


	//Storing results...);
	
	int count = 0;
	FILE* tempFile;

	
	tempFile = fopen("STAT_temp.temp", "r" );
	int ch;
	fseek(tempFile,0,SEEK_END);
	count = ftell(tempFile);
	char *result;
	fseek(tempFile,0,SEEK_SET);
	
	result = (char*)calloc(count, sizeof(char));	

	if(result == NULL)
    	printf("ERROR MEMORY LOL");
    
    fread(result, sizeof(char), count, tempFile);
	

	
	/*
	while( !feof( tempFile ) && tempFile )
	{
	ch = fgetc(tempFile);
	        if (ch == EOF)
	            break;
	        result[count] = 
	        ++count;
	}
	*/

	fclose( tempFile ); 
	//printf("The file contains this\n\n%s", result);

	
	//result = (char *)malloc(strlen(result) + count);

	
	/*
	char line[100];
	FILE *tempFile2;
	  if((tempFile2 = fopen("STAT_temp.temp", "r")) == NULL) {
	    printf("Error Opening File.\n");
	    exit(1);
	  }
	  	
	while( fgets(line, sizeof(line), tempFile2) != NULL ) {
		
	    strcat(result,line);
	  }
	 
	fclose(tempFile2);
	*/
	
	//Removing temp file
	remove("STAT_temp.temp");

	return result;
}

void stat_handle()
{
	//Parse the packet to find out username and flag

	char *userName = "burnsh";
	char *directory = "/home/burnsh/";
	int flag = 2;

	if(flag == ST_WHO) //show users logged on
	{
		//char *finalCommand = "finger -lp > STAT_temp.temp";
		char *finalCommand = "users > STAT_temp.temp";

		char *results = runCommand_getResults(finalCommand);		
		
		printf("%s", results);
	}

	if(flag == ST_PROC) //show processes of user
	{
		char *finalCommand = "";
		char *command = "ps -ef | grep '";
		char *endCommand = "' > STAT_temp.temp";
  			finalCommand = (char *)calloc(strlen(userName) + strlen(command) + 
			strlen(endCommand)+ strlen(finalCommand) + 1, sizeof(char));

		strcat(finalCommand,command);
		strcat(finalCommand,userName);
		strcat(finalCommand,endCommand);

		char *results = runCommand_getResults(finalCommand);
		printf("%s", results);
	}
	
	if(flag == ST_LS) //show directory
	{
		char *finalCommand = "";
		char *command = "ls ";
		char *endCommand = " > STAT_temp.temp";
  			finalCommand = (char *)calloc(strlen(userName) + strlen(command) + 
			strlen(endCommand)+ strlen(finalCommand) + 1, sizeof(char));

		strcat(finalCommand,command);
		strcat(finalCommand,directory);
		strcat(finalCommand,endCommand);

		char *results = runCommand_getResults(finalCommand);
		printf("%s", results);
	}
	
	return;
}

