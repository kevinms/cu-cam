#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int split_string (char *buf, char **seg);
char *p_strcpy (char *str);
char *p_strncpy (char *str, int len);

#endif /* !_UTILS_H_ */

/*
	count = split_string (full_buf, buf);
	for (i=0; i<count; i++)
	{
*/
