#include "gruppo.h"

int sqlMain(char* filebuffer)
{
    regex_t rgx;
    regmatch_t matches[SIZE_OF_MATCHES*MAX_MATCHES];
    int i,ret, nmatches = 0;
    char buffer[100];
    const char* regex = "<SQL filename=\"([^<>\"]*.sqlite)\">([^/]*)</SQL>";    
    sqlite3* db = NULL;
    char *zErrMsg = 0;
    int rc;
    char sql[100];

    ret = regcomp(&rgx, regex, REG_ICASE | REG_EXTENDED);
    if(ret)
    {
        	regerror(ret, &rgx, buffer, 100);
        	printf("regcomp() failed with '%s'\n", buffer);
        	exit(EXIT_FAILURE);      
    }
	puts("-------------REGEX----------");
	char *tmp = filebuffer;
        regmatch_t *tmpmatches = matches;
    while(!(ret =regexec(&rgx, tmp, SIZE_OF_MATCHES, tmpmatches, 0)))
	{
		printf("at %p in %p\n",tmp,tmpmatches);
		for(i = 0; i < SIZE_OF_MATCHES && (tmpmatches[i].rm_eo != -1); i++)
                {
                  printf("\t%d: %.*s\n",
                          i,
                          tmpmatches[i].rm_eo - tmpmatches[i].rm_so,
                          tmp + tmpmatches[i].rm_so);
                }
		tmp += tmpmatches[0].rm_eo;
                tmpmatches += 3;
                nmatches++;
	}
	if(ret)
	{
		regerror(ret, &rgx, buffer, 100);
		printf("regexec() failed with '%s'\n", buffer);
		if(ret != REG_NOMATCH)
			exit(EXIT_FAILURE);
	}
    regfree(&rgx);
	puts("----------------------------");

/******************************************************************************************************************************************/
/******************************************************************************************************************************************/

	sqlquery queries[nmatches];
	int j;
	for(i = 1,j = 1; j <= nmatches; j++,i+=3)	
	{
		strncpy(sql,filebuffer + matches[i].rm_so,matches[i].rm_eo - matches[i].rm_so);
                puts(sql);
		if(sqlite3_open(sql, &db))
		{
		    fprintf(stderr, "\nCan't open database: %s\n", sqlite3_errmsg(db));
		    exit(0);
		}
		else
		{
		    fprintf(stdout, "\nOpened database successfully\n");
			strncpy(sql,filebuffer + matches[i+1].rm_so,matches[i+1].rm_eo - matches[i+1].rm_so);
			queries[i-1].noHeader = -1;
			puts(sql);
			rc = sqlite3_exec(db, sql, sqlcallback, (void*)&(queries[i-1]), &zErrMsg);
			if( rc != SQLITE_OK )
			{
		    	fprintf(stderr, "\nN°:%d SQL error: %d %s\n",i, rc, zErrMsg);
		    	sqlite3_free(zErrMsg);
			}
			else
		    	fprintf(stdout, "\nOperation done successfully\n");
			sqlite3_close(db);
		}
	}

/******************************************************************************************************************************************/
/******************************************************************************************************************************************/

	int lung = strlen(filebuffer);
	lung += strlen("<TABLE BORDER=\"1\">\n\t\t\t</TABLE>")*nmatches;
	for(i = 0; i < nmatches; i+=3)
	{
		lung -= (matches[i].rm_eo - matches[i].rm_so);
		lung += strlen(queries[i].header) + strlen(queries[i].table);
	}

    char finale[lung]; //creo nuovo buffer con la lunghezza adatta
    strncpy(finale,filebuffer,matches[0].rm_so); //copio nel nuovo buffer tutto quello che ce prima del tag sql
	for(i = 0; i < nmatches; i++)
	{
		strcat(finale,"<TABLE BORDER=\"1\">\n");
    	strcat(finale,queries[i].header); //concateno la tabella
		strcat(finale,queries[i].table);
		strcat(finale,"\t\t\t</TABLE>");
	}
    strcat(finale,(filebuffer + matches[0].rm_so) + (matches[0].rm_eo - matches[0].rm_so)); //concateno il contenuto restante del file html
    puts("-------------NUOVO FILE HTML----------");
    //printf("%s",finale);
	FILE* fp;
    fp = fopen("result.html","w");
    fputs(finale,fp);
    fclose(fp);

	return 0;
}
static int sqlcallback(void *data, int argc, char **argv, char **azColName)
{
	sqlquery* query = (sqlquery*)data;
	int i;
	
    if(query->noHeader)
    {
		strcpy(query->header, "<TR>");
        for(i=0; i<argc; i++)
        {
            strcat(query->header, "<TH>");
            strcat(query->header, azColName[i]);
            strcat(query->header, "</TH>");
        }
		strcat(query->header, "</TR>");
        query->noHeader = 0;
    }

	strcat(query->table, "<TR>");
    for(i=0; i<argc; i++)
    {
        strcat(query->table, "<TD>");
        strcat(query->table, argv[i] ? argv[i] : "NULL");
        strcat(query->table, "</TD>");
    }
	strcat(query->table, "</TR>\n");
    return 0;
}
