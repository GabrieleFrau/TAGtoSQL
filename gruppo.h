#ifndef GRUPPO_H_INCLUDED
#define GRUPPO_H_INCLUDED
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

#define SIZE_OF_MATCHES 3
#define MAX_FILE_LENGTH 4096
#define TRUE 1
#define FALSE 0
#define MATCH_TOTALE 0
#define MATCH_NOMEDB 1
#define MATCH_QUERY 2

typedef struct squery
{
    int noHeader;
    char header[512];
    char table[512];
} sqlquery;

int sqlMain( char *filebuffer );
static int sqlcallback( void *data, int argc, char **argv, char **azColName );

#endif
