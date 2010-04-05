/* 
 * File:   stat.h
 * Used by the server and client to send stat requests and handle them.
 */

#ifndef _STAT_H
#define	_STAT_H

	#define ST_WHO  0  // Show who is logged on
	#define ST_PROC 1  // Show process a user has running
	#define ST_LS   2  // List the contents of a server

	void stat_handle();
	void stat_request();

#endif	/* _STAT_H */

