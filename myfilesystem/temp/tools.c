#include "filesym.h"

extern struct fcb *fcbs;
extern struct index *indexs;//[LEN];
extern int semid;
extern struct directory *curdir;
extern struct user curuser;
extern struct user *usernode;//[USERLEN];
extern struct group *groupnode;//[GROUPLEN];
extern char   *filenode;
extern char command_tab[COMMANDLEN][10];



void shminit()
{
	int i;
	int fcb_shmid, index_shmid, user_shmid, group_shmid, filenode_shmid;
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
	if((semid = semget(992, LEN, 0666|IPC_CREAT)) < 0)
	{
		perror("semget");
		exit(1);
	}	
	for(i = 0; i < LEN; i++)
	{
		semctl(semid, i, SETVAL, 1);
	}
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
				if(fcbs[curdir->inode].ext[k] == j)
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
		acc = 2;
	}
	else if(file->gid == curuser.gid)
	{
		acc = 1;
	}
	else
	{
		acc = 0;
	} 
	switch(op)
	{
		case 'x':
			if(file->access[acc] != '1' && file->access[acc] != '2' && file->access[acc] != '3')
			{
				return 1;			
			}
			else return 0;
		case 'w':
			if(file->access[acc] != '1' && file->access[acc] != '4' && file->access[acc] != '5')
			{
				return 1;
			}
			else return 0;
		case 'r':
			if(file->access[acc] != '2' && file->access[acc] != '4' && file->access[acc] != '6')
			{
				return 1;
			}
			else return 0;
		default : return 0; break;
	}

}
