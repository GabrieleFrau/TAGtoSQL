#include "gruppo.h"

int sqlMain ( char *filebuffer )
{
    regex_t rgx;
    regmatch_t matches[SIZE_OF_MATCHES];
    int i, ret, rc, first = TRUE;
    const char *regex = "<SQL filename=\"([^<>\"]*.sqlite)\">([^/]*)</SQL>";
    sqlite3 *db = NULL;
    char *zErrMsg = 0, *tmp = filebuffer;
    char finale[MAX_FILE_LENGTH] = { '\0' };
    ret = regcomp ( &rgx, regex, REG_ICASE | REG_EXTENDED );
    if ( ret )
    {
        char buffer[100];
        regerror ( ret, &rgx, buffer, 100 );
        printf ( "regcomp() failed with '%s'\n", buffer );
        exit ( EXIT_FAILURE );
    }
    printf ( "\n-----------------------------------------------------------------------------------------------------\n" );
    while ( ! ( ret = regexec ( &rgx, tmp, SIZE_OF_MATCHES, matches, 0 ) ) )
    {
        char database[100];
        char sql[100];
        sqlquery query;
        for ( i = 0; ( matches[i].rm_eo != -1 ) && i < SIZE_OF_MATCHES ; i++ )
        {
            printf ( "\t%d: %.*s\n",
                     i,
                     matches[i].rm_eo - matches[i].rm_so,
                     tmp + matches[i].rm_so );
        }
        if ( first == TRUE )
        {
            strncpy ( finale, tmp, matches[MATCH_TOTALE].rm_so );
            first = FALSE;
        }
        else
        {
            strncat ( finale, tmp, matches[MATCH_TOTALE].rm_so );
        }
        strncpy ( database, tmp + matches[MATCH_NOMEDB].rm_so, matches[MATCH_NOMEDB].rm_eo - matches[MATCH_NOMEDB].rm_so );
        printf ( "DATABASE: %s\n", database );
        if ( sqlite3_open ( database, &db ) )
        {
            fprintf ( stderr, "\nCan't open database: %s\n", sqlite3_errmsg ( db ) );
            exit ( EXIT_FAILURE );
        }
        printf ( "\nOpened database successfully\n" );
        strncpy ( sql, tmp + matches[MATCH_QUERY].rm_so, matches[MATCH_QUERY].rm_eo - matches[MATCH_QUERY].rm_so );
        printf ( "QUERY: %s\n", sql );
        query.noHeader = TRUE;
        rc = sqlite3_exec ( db, sql, sqlcallback, ( void * ) & ( query ), &zErrMsg );
        if ( rc != SQLITE_OK )
        {
            fprintf ( stderr, "\nN°:%d SQL error: %d %s\n", i, rc, zErrMsg );
            sqlite3_free ( zErrMsg );
        }
        printf ( "\nOperation done successfully\n" );
        sqlite3_close ( db );
        strcat(finale,"<TABLE BORDER = \"1\"");
        strcat(finale,query.header);
        strcat(finale,query.table);      
        strcat(finale,"</TABLE>");
        tmp += matches[MATCH_TOTALE].rm_eo;
        printf ( "\n-----------------------------------------------------------------------------------------------------\n" );
    }
    strncat(finale, tmp, strlen(tmp) - 1);
    FILE * fp = fopen("result.html","w");
    if(fp != NULL)
    {
      fputs(finale,fp);
      fclose(fp);
      return EXIT_SUCCESS;
    }
    else
      return EXIT_FAILURE;
}

static int sqlcallback ( void *data, int argc, char **argv, char **azColName )
{
    sqlquery *query = ( sqlquery * ) data;
    int i;
    if ( query->noHeader == TRUE )
    {
        strcpy ( query->header, "<TR>" );
        for ( i = 0; i < argc; i++ )
        {
            strcat ( query->header, "<TH>" );
            strcat ( query->header, azColName[i] );
            strcat ( query->header, "</TH>" );
        }
        strcat ( query->header, "</TR>" );
        query->noHeader = FALSE;
    }
    strcpy ( query->table, "<TR>" );
    for ( i = 0; i < argc; i++ )
    {
        strcat ( query->table, "<TD>" );
        strcat ( query->table, argv[i] ? argv[i] : "NULL" );
        strcat ( query->table, "</TD>" );
    }
    strcat ( query->table, "</TR>" );
    return EXIT_SUCCESS;
}
