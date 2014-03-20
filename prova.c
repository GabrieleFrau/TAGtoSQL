#include "gruppo.c"

int main()
{
	int i = 0;
	char filebuffer[4096];
	FILE* fp = fopen("test.html","r");
	while ((filebuffer[i] = (char)fgetc(fp)) && filebuffer[i++] != EOF);
    fclose(fp);
	sqlMain(filebuffer);
	return 0;
}
