#include "filesym.h"

extern char *filenode;//[2049];
extern int mapid;
char str[5120];
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


int write_real(int num, char *str, int size)  //写物理块，最多2048块，每块512B，前4块为位示图
{
	FILE *f = fopen("real","r+"); //一定不能用a,用a的话fseek会失效
	fseek(f, 0, SEEK_SET);  //将文件指针置在文件开头
	fseek(f, (num - 1) * 512, SEEK_CUR);//将文件指针偏转到对应物理块开头
	fwrite(str, size, 1, f);  //写入文件
	fputc(0,f);  //写入结束符
	fclose(f);
	return 1;
}


void write_filenode()
{
	FILE *f = fopen("real","r+");
	fseek(f,0,SEEK_SET);  //将文件指针置在文件开头
	fwrite(filenode,2048,1,f);  //写入位示图内容
	fclose(f);
}

void read_real(int num)
{
	FILE *f = fopen("real", "r");
	char ch, temp[512];
	int i = 0;
	memset(temp, 0, 512);
	fseek(f, 0, SEEK_SET);
	fseek(f, (num - 1) * 512, SEEK_CUR);
	ch = fgetc(f);
	while(ch != 0)
	{
		temp[i] = ch;
		ch = fgetc(f);
		i++;
	}
	strcat(str, temp);
	return;
}

void read_block(struct fcb *file)
{
	int i;
	memset(str, 0, 5120);
	for(i = 0; i < getsize(file->ext); i++)
	{
		if(i == 10)
		{
			int retval = 0;
			FILE *fp = fopen("./real", "r+");
			fseek(fp, (file->ext[10] - 1) * 512, SEEK_CUR);
			retval = getw(fp);
			while(retval != 0 && retval != -1)
			{
				read_real(retval);					
			}
		}
		if(file->ext[i])
		{
			read_real(file->ext[i]);
		}
		else
		{
			break;
		}
	}
	strcat(str, "\n");
	message_send(str);	
}





void write_last_ext(struct fcb *file, char *contents)
{
	int i = 0, j = 0, k = 0;
	FILE *fp;
	struct sembuf sembuffer;
	fp = fopen("./real", "r+");
	if(file->ext[10] == 0)
	{
		for(k = 0; k < getsize(filenode); k++)
		{
			sembuffer.sem_num = 0;
			sembuffer.sem_op = -1;
			sembuffer.sem_flg = SEM_UNDO;
			semop(mapid, &sembuffer, 1);
			if(filenode[k] == '0')
			{
				filenode[k] = '1';
				file->ext[10] = k + 1;
				sembuffer.sem_op = 1;	
				semop(mapid, &sembuffer, 1);
				break;
			}	
			sembuffer.sem_op = 1;	
			semop(mapid, &sembuffer, 1);
		}
		if(k == getsize(filenode))
		{
			return ;
		}
		fseek(fp, k * 512, SEEK_CUR);
		for(i = 0; i < strlen(contents); i += 511)
		{
			for(j = 0; j < getsize(filenode); j++)
			{
				sembuffer.sem_num = 0;
				sembuffer.sem_op = -1;
				sembuffer.sem_flg = SEM_UNDO;
				semop(mapid, &sembuffer, 1);
				if(filenode[j] == '0')
				{
					filenode[j] = '1';
					sembuffer.sem_op = 1;	
					semop(mapid, &sembuffer, 1);
					putw(j + 1, fp);
					write_real(j + 1, contents + i, strlen(contents + i) > 511 ? 511 : strlen(contents + i));
					break;
				}	
				sembuffer.sem_op = 1;	
				semop(mapid, &sembuffer, 1);
				
			}
		//	putw(0, fp);
		}
		putw(0, fp);
	}
	else
	{
		int oldval = 0, retval = 0, count = 0;
		char ch;
		fp = fopen("./real", "r+");
		fseek(fp, (file->ext[10] - 1) * 512, SEEK_CUR);
		retval = getw(fp);
		while(retval != 0 && retval != -1)
		{
			count++;
			oldval = retval;
			retval = getw(fp);
		}		
		if(count == 127)
		{
			count = 0;
			fseek(fp, (oldval - 1) * 512, SEEK_CUR);
			ch = fgetc(fp);
			while(ch)
			{
				count++;
				ch = fgetc(fp);
			}
			if(count == 511)
			{
				return ;//full
			}
			else if((511 - count) >= strlen(contents))
			{
				fseek(fp, -1, SEEK_CUR);
				fwrite(contents, strlen(contents), 1, fp);
				fputc(0, fp);
				fclose(fp);			
				return ;
			}
			else
			{
				return ;//full
			}
		}
		else
		{
			int temp = count;
			count = 0;
			fseek(fp, (oldval - 1) * 512, SEEK_CUR);
			ch = fgetc(fp);
			while(ch)
			{
				count++;
				ch = fgetc(fp);
			}
			if((511 - count) >= strlen(contents))
			{
				
				fseek(fp, -1, SEEK_CUR);
				fwrite(contents, strlen(contents), 1, fp);
				fputc(0, fp);
				fclose(fp);			
				return ;
			}
			else
			{
				fseek(fp, -1, SEEK_CUR);
				fwrite(contents, (511 - count), 1, fp);
				fputc(0, fp);
				fseek(fp, (file->ext[10] - 1) * 512, SEEK_SET);
				fseek(fp, temp * 4, SEEK_CUR);
				for(i = (511 - count); i < strlen(contents); i++)
				{
					for(j = 0; j < getsize(filenode); j++)
					{
						sembuffer.sem_num = 0;
						sembuffer.sem_op = -1;
						sembuffer.sem_flg = SEM_UNDO;
						semop(mapid, &sembuffer, 1);
						if(filenode[j] == '0')
						{
							filenode[j] = '1';
							sembuffer.sem_op = 1;
							semop(mapid, &sembuffer, 1);
							write_real(j + 1, contents + i, strlen(contents + i) > 511 ? 511 : strlen(contents + i));
							
							putw(j + 1, fp);
							break;
						}
						sembuffer.sem_op = 1;
						semop(mapid, &sembuffer, 1);
					}			
				}
			}
		}
	}
}






void write_block(struct fcb *file, char *contents)
{
	int i = 0, j = 0, k = 0;
	int end, count = 0;
	char ch;
	int content_size = strlen(contents);	
	struct sembuf sembuffer;

	file->f_size += content_size; 
	if(file->ext[0] == 0)
	{
		for(i = 0; i < content_size; i += 511)
		{
			if((i / 511) == 10)
			{
				write_last_ext(file, contents + i);
				return;
			}
			for(j = 0; j < getsize(filenode); j++)
			{
				sembuffer.sem_num = 0;
				sembuffer.sem_op = -1;
				sembuffer.sem_flg = SEM_UNDO;
				semop(mapid, &sembuffer, 1);
				if(filenode[j] == '0')
				{
					filenode[j] = '1';
					sembuffer.sem_op = 1;	
					semop(mapid, &sembuffer, 1);
					if(strlen(contents + i) > 511)
					{
						write_real(j + 1, contents + i, 511);					
					}
					else
					{
						write_real(j + 1, contents + i, strlen(contents + i));					
					}
					file->ext[i/511] = j + 1;
					break;	
				}			
				sembuffer.sem_op = 1;	
				semop(mapid, &sembuffer, 1);
			}				
		}
		write_filenode();
	}
	else
	{
		for(i = 1; i < getsize(file->ext); i++)
		{
			if(i == 10)
			{
				write_last_ext(file, contents +k);
				return ;
			}
			else if(file->ext[i] == 0)
			{
				end = file->ext[i - 1];
				FILE *f	 = fopen("real", "r+");
				fseek(f, (end - 1) * 512, SEEK_CUR);
				ch = fgetc(f);
				while(ch)
				{
					count++;
					ch = fgetc(f);
				}	
				if(count == 511)
				{
					fclose(f);
					for(k = 0; k < content_size; k += 511)
					{
						for(j = 0; j < getsize(filenode); j++)
						{
							sembuffer.sem_num = 0;
							sembuffer.sem_op = -1;
							sembuffer.sem_flg = SEM_UNDO;
							semop(mapid, &sembuffer, 1);
							if(filenode[j] == '0')
							{
								filenode[j] = '1';
								sembuffer.sem_op = 1;
								semop(mapid, &sembuffer, 1);
								write_real(j + 1, contents + k, strlen(contents + k) > 511 ? 511 : strlen(contents + k));
								file->ext[i++] = j + 1;
								break;							
							}
							sembuffer.sem_op = 1;
							semop(mapid, &sembuffer, 1);
						}	
					}
					break;
				}
				if((511 - count) >= content_size)
				{
					fseek(f, -1, SEEK_CUR);
					fwrite(contents, content_size, 1, f);
					fputc(0, f);
					fclose(f);
				}		
				else
				{
					fseek(f, -1, SEEK_CUR);
					fwrite(contents, 511 - count, 1, f);
					fputc(0, f);
					fclose(f);	
					for(k = 511 - count; k < content_size; k += 511)
					{
						for(j = 0; j < getsize(filenode); j++)
						{
							sembuffer.sem_num = 0;
							sembuffer.sem_op = -1;
							sembuffer.sem_flg = SEM_UNDO;
							semop(mapid, &sembuffer, 1);
							if(filenode[j] == '0')
							{
								filenode[j] = '1';
								sembuffer.sem_op = 1;
								semop(mapid, &sembuffer, 1);
								write_real(j + 1, contents + k, strlen(contents + k) > 511 ? 511 : strlen(contents + k));
								file->ext[i++] = j + 1;
								break;
							}
							sembuffer.sem_op = 1;
							semop(mapid, &sembuffer, 1);
						}
					}	
				}
				write_filenode();		
				break;
			}
		}
	}

}



















