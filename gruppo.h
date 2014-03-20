#ifndef GRUPPO_H_INCLUDED
#define GRUPPO_H_INCLUDED
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

#define SIZE_OF_MATCHES 3
#define MAX_MATCHES 3
typedef struct squery
	{
		int noHeader;
		char header[256];
		char table[256];	
	}sqlquery;

int sqlMain(char* filebuffer);
static int sqlcallback(void *data, int argc, char **argv, char **azColName);

#endif
