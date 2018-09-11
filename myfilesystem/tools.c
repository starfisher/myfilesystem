#include "filesym.h"

extern struct fcb *fcbs;
extern struct index *indexs;//[LEN];
extern int semid;
extern int mapid;
extern struct directory *curdir;
extern struct user curuser;
extern struct user *usernode;//[USERLEN];
extern struct group *groupnode;//[GROUPLEN];
extern char   *filenode;
extern char command_tab[COMMANDLEN][10];
int fcb_shmid, index_shmid, user_shmid, group_shmid, filenode_shmid;


char buf[4096];
void shmrelease()
{
	shmctl(fcb_shmid, IPC_RMID, NULL);
	shmctl(index_shmid, IPC_RMID, NULL);
	shmctl(user_shmid, IPC_RMID, NULL);
	shmctl(group_shmid, IPC_RMID, NULL);
	shmctl(filenode_shmid, IPC_RMID, NULL);
}



void shminit()
{
	int i;
	if((fcb_shmid = shmget(906, sizeof(struct fcb) * LEN, 0666|IPC_CREAT)) < 0)
	{
		perror("fcb_shmid");
		exit(1);
	}	
	if((fcbs = shmat(fcb_shmid, 0, 0)) < (char *)0)
	{
		perror("fcb_shmat");
		exit(1);
	}
	memset(fcbs, 0, sizeof(struct fcb) * LEN);
	if((index_shmid = shmget(907, sizeof(struct index) * LEN, 0666|IPC_CREAT)) < 0)
	{
		perror("index_shmid");
		exit(1);
	}	
	if((indexs = shmat(index_shmid, 0, 0)) < (char *)0)
	{
		perror("index_shmat");
		exit(1);
	}

	memset(indexs, 0, sizeof(struct index) * LEN);
	if((user_shmid = shmget(908, sizeof(struct user) * USERLEN, 0666|IPC_CREAT)) < 0)
	{
		perror("user_shmid");
		exit(1);
	}	
	if((usernode = shmat(user_shmid, 0, 0)) < (char *)0)
	{
		perror("user_shmat");
		exit(1);
	}
	memset(usernode, 0, sizeof(struct user) * USERLEN);

	if((group_shmid = shmget(909, sizeof(struct group) * GROUPLEN, 0666|IPC_CREAT)) < 0)
	{
		perror("group_shmid");
		exit(1);
	}	
	if((groupnode = shmat(group_shmid, 0, 0)) < (char *)0)
	{
		perror("group_shmat");
		exit(1);
	}
	memset(groupnode, 0, sizeof(struct group) * GROUPLEN);
	if((filenode_shmid = shmget(910, 2049, 0666|IPC_CREAT)) < 0)
	{
		perror("filenode_shmid");
		exit(1);
	}	
	if((filenode = shmat(filenode_shmid, 0, 0)) < (char *)0)
	{
		perror("filenode_shmat");
		exit(1);
	}
	memset(filenode, 0, 2049);
	if((semid = semget(992, LEN, 0666|IPC_CREAT)) < 0)
	{
		perror("semget");
		exit(1);
	}	
	for(i = 0; i < LEN; i++)
	{
		semctl(semid, i, SETVAL, 1);
	}
	if((mapid = semget(777, 1, 0666|IPC_CREAT)) < 0)
	{
		perror("semget mapid");
		exit(1);
	}	
	semctl(mapid, 0, SETVAL, 1);
//	for(i = 0; i < LEN; i++)
//	{
//		printf("%d ", semctl(semid, i, GETVAL));
//	}
}



BOOL is_empty_file(char *f_name)
{
	FILE *fp;
	char ch;
	if((fp = fopen(f_name, "r")) == NULL)
	{
		return TRUE;
	}
	ch = fgetc(fp);
	fp = NULL;
	if(ch == EOF)
	{
		return TRUE;
	}	
	else
	{
		return FALSE;
	}
}

struct tm get_cur_time()
{
	time_t t = time(NULL);
	struct tm *tm;
	tm = localtime(&t);
	tm->tm_year += 1900;
	tm->tm_mon++;
	
	return *tm;

}




int getch(){
 int c=0;
 struct termios org_opts, new_opts;
 int res=0;
    //-----  store old settings -----------
     res=tcgetattr(STDIN_FILENO, &org_opts);
     assert(res==0);
   //---- set new terminal parms --------
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c=getchar();
   //------  restore old settings ---------
  res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
  assert(res==0);
  return c;
}


int get_inode_by_name(char *name)
{
	int index = -1;
	int j, k;
	for(j = 0; j < LEN; j++)
	{
		if(indexs[j].flag == 0)
		{
			continue;
		}		
		if(!strcmp(indexs[j].i_name, name))
		{
			for(k = 0; k < sizeof(fcbs[0].ext)/sizeof(fcbs[0].ext[0]); k++)
			{
				if(k == 10 && fcbs[curdir->inode].ext[k] != 0)
				{
					int temp = fcbs[curdir->inode].ext[k];
					if(temp)
					{
						FILE *fp = fopen("./real", "r+");
						int retval = 0;
						fseek(fp, (temp - 1) * 512, SEEK_CUR);
						retval = getw(fp);
						while(retval != 0 && retval != -1)
						{
							if(retval == j)
							{
								index = retval;
								break;
							}
							retval = getw(fp);
						}
						free(fp);
					}					
				}
				else if(fcbs[curdir->inode].ext[k] == j)
				{
					index = j;
					break;
				}

			}	
		}
	}
	return index;
} 



int auth(char op, struct fcb *file)
{
	int acc;
	if(curuser.uid == 1)
	{
		return 1;
	}
	if(file->uid == curuser.uid)
	{
		acc = 0;
	}
	else if(file->gid == curuser.gid)
	{
		acc = 1;
	}
	else
	{
		acc = 2;
	} 
	switch(op)
	{
		case 'r':
			if(file->access[acc] != '1' && file->access[acc] != '2' && file->access[acc] != '3' && file->access[acc] != '0')
			{
				return 1;			
			}
			else return 0;
		case 'w':
			if(file->access[acc] != '1' && file->access[acc] != '4' && file->access[acc] != '5' && file->access[acc] != '0')
			{
				return 1;
			}
			else return 0;
		case 'x':
			if(file->access[acc] != '2' && file->access[acc] != '4' && file->access[acc] != '6' && file->access[acc] != '0')
			{
				return 1;
			}
			else return 0;
		default : return 0; break;
	}

}


void auth_disp(char *access, char *temp)
{
	int i;
	for(i = 0; i < strlen(access); i++)
	{
		switch(*(access + i))
		{	
			case '0':strcat(temp, "---");break;
			case '1':strcat(temp, "--x");break;
			case '2':strcat(temp, "-w-");break;
			case '3':strcat(temp, "-wx");break;
			case '4':strcat(temp, "r--");break;
			case '5':strcat(temp, "r-x");break;
			case '6':strcat(temp, "rw-");break;
			case '7':strcat(temp, "rwx");break;
		}
	}
}

void user_disp(int uid, char *temp)
{
	int i;
	for(i = 0; i < USERLEN; i++)
	{
		if(usernode[i].uid == uid)
		{
			strcpy(temp, usernode[i].username);
			return ;
		}
	}
	strcpy(temp, "NULL");
	return ;
}

void group_disp(int gid, char *temp)
{
	int i;
	for(i = 0; i < GROUPLEN; i++)
	{
		if(groupnode[i].gid == gid)
		{
			strcpy(temp, groupnode[i].g_name);
			return ;
		}
	}	
	strcpy(temp, "NULL");
	return ;
}


void fcb_disp(int num, int access, int more)
{
	char temp[10];
	if(more)
	{
		buf[strlen(buf)] = (fcbs[num].f_type == 'f' ? '-' : fcbs[num].f_type);
		memset(temp, 0, sizeof(temp));
		auth_disp(fcbs[num].access, temp);
		strcat(strcat(buf, temp), "\t");
				
		memset(temp, 0, sizeof(temp));
		user_disp(fcbs[num].uid, temp);
		strcat(strcat(buf, temp), "\t");
			
		memset(temp, 0, sizeof(temp));
		group_disp(fcbs[num].gid, temp);	
		strcat(strcat(buf, temp), "\t");

		memset(temp, 0, sizeof(temp));
		//itoa(fcbs[file->ext[k]].f_size, temp, 10);
		sprintf(temp, "%d", fcbs[num].f_size);
		strcat(strcat(buf, temp), "\t");
				
		memset(temp, 0, sizeof(temp));
		sprintf(temp, "%d", fcbs[num].tm.tm_hour);
		strcat(strcat(buf, temp), ":");	
		memset(temp, 0, sizeof(temp));
		sprintf(temp, "%d", fcbs[num].tm.tm_min);
		strcat(strcat(buf, temp), ":");	
		memset(temp, 0, sizeof(temp));
		sprintf(temp, "%d", fcbs[num].tm.tm_sec);
		strcat(strcat(buf, temp), "\t");

		memset(temp, 0, sizeof(temp));
		if(auth('r', &fcbs[num]))
		{
			strcat(temp, indexs[num].i_name);	
		}
		else
		{
			strcat(temp, "??????");
		}
		strcat(buf, access ? temp : "??????");	
		strcat(buf, "\n");
	}
	else
	{
		memset(temp, 0, sizeof(temp));
		if(auth('r', &fcbs[num]))
		{
			strcat(temp, indexs[num].i_name);	
		}
		else
		{
			strcat(temp, "??????");
		}
		strcat(buf, access ? temp : "??????");	
		strcat(buf, "\t");	
	}

}


void fcbs_disp(struct fcb *file, int access, int more)
{
	int k;
	memset(buf, 0, sizeof(buf));
	for(k = 0; k < sizeof(file->ext)/sizeof(file->ext[0]); k++)
	{
		if(k == 10 && file->ext[k] != 0)
		{
			int retval = 0;
			FILE *fp = fopen("./real", "r+");		
			fseek(fp, (file->ext[k] - 1) * 512, SEEK_CUR);
			retval = getw(fp);
			while(retval != 0 && retval != -1)
			{
				fcb_disp(retval, access, more);
				retval = getw(fp);	
			}
			free(fp);
		}
		else if(file->ext[k])
		{
			fcb_disp(file->ext[k], access, more);
		}
	}
	strcat(buf, "\n");
	message_send(buf);
	return ;
}

