#include "filesym.h"

extern char *filenode;//[2049];

void init_filenode()
{
	FILE *fp = fopen("real", "w");
	filenode[0] = '1';	
	filenode[1] = '1';	
	filenode[2] = '1';	
	filenode[3] = '1';
	fseek(fp, 0, SEEK_SET);
	fwrite(filenode, 2049, 1, fp);
	fclose(fp);
	return ;	
}

void read_filenode()
{
	FILE *fp = fopen("real", "rb");
	fseek(fp, 0, SEEK_SET);
	fread(filenode, 2048, 1, fp);
	fclose(fp);
	return ;		
}

