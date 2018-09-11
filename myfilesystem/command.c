#include "filesym.h"

extern struct directory *curdir;
extern struct index *indexs;//[LEN];
extern struct user *usernode;//[USERLEN];
extern struct group *groupnode;//[USERLEN];
extern struct fcb *fcbs;//[LEN];
extern struct user curuser;
extern struct directory dirchain;
extern char *filenode;
extern int semid;
extern int mapid;

char buf[1024];
struct sembuf sembuffer;

BOOL login(char *username, char *passwd)
{
	int i;
	for(i = 0; i < USERLEN; i++)
	{
		if((!strcmp(username, usernode[i].username)) && (!strcmp(passwd, usernode[i].passwd)) )
		{
			curuser.uid = usernode[i].uid;
			curuser.gid = usernode[i].gid;
			strcpy(curuser.username, usernode[i].username);
			strcpy(curuser.passwd, "******");
			curdir = &dirchain;
			curdir->next = NULL;
			return TRUE;
			
		}
	}
	return FALSE;
}


void pwd()
{
	struct directory *p = &dirchain;
	memset(buf, 0, sizeof(buf));
	while(p != NULL)
	{
		strcat(buf, p->d_name);
		if(!strcmp(p->d_name, "/"))
		{
			;	
		}	
		else
		{
			strcat(buf, "/");
		}
		p = p->next;
	
	}
	if(strlen(buf) > 1)
	{
		buf[strlen(buf) - 1] = '\n';
	}
	else
	{
		strcat(buf, "\n");
	}
	message_send(buf);
	return ;
}


struct fcb* open_file(char *f_name)
{
	int index = get_inode_by_name(f_name);
	if(index == -1)
	{
		return NULL;
	}
	return &fcbs[index];
}


void ls(int type)
{
	int index = curdir->inode;
	int access = 0;
	if(auth('r', &fcbs[index]))
	{
		access = 1;
	}
	fcbs_disp(&fcbs[index], access, type);
}


void mkdir(char *param1, char *param2, char type)
{
	char name[10];
	char access[4];
	int flag = 0;
	int i, j, k, index = -1;
	if(atoi(param1))
	{
		strcpy(access, param1);
		flag++;
	}
	else
	{
		strcpy(name, param1);
	}
	if(atoi(param2))
	{
		strcpy(access, param2);
		flag++;
	}
	else
	{
		strcpy(name, param2);
	}
	if(flag != 1)
	{
		message_send("mkdir:param unknow!\n");
		return ;
	}
	index = get_inode_by_name(name);
	if(index != -1)
	{
		message_send("mkdir:file is already exist.\n");
		return ;
	}	
	for(i = 0; i < LEN; i++)
	{
		if(indexs[i].flag == 0)
		{
			sembuffer.sem_num = i;
			sembuffer.sem_op = -1;
			sembuffer.sem_flg = SEM_UNDO;
			semop(semid, &sembuffer, 1);
			if(indexs[i].flag != 0)
			{
				sembuffer.sem_op = 1;	
				semop(semid, &sembuffer, 1);
				continue ;
			}
			indexs[i].inode = i;
			strcpy(indexs[i].i_name, name);
			indexs[i].flag = 1;

			fcbs[i].inode = i;
			fcbs[i].f_type = type;  
			fcbs[i].uid = curuser.uid;
			fcbs[i].gid = curuser.gid;
			for(k = 0; k < getsize(fcbs[i].ext); k++)
			{
				fcbs[i].ext[k] = 0;
			}
			strcpy(fcbs[i].access, access);
			fcbs[i].access[3] = '\0';
			fcbs[i].tm = get_cur_time();
			fcbs[i].flag = 1;
			break;
		}	
	//	sembuffer.sem_op = 1;	
	//	emop(semid, &sembuffer, 1);
	}
	for(k = 0; k < sizeof(fcbs[curdir->inode].ext)/sizeof(fcbs[curdir->inode].ext[0]); k++)
	{
		if(k == 10)
		{
			if(fcbs[curdir->inode].ext[k] == 0)
			{
				for(j = 0; j < getsize(filenode); j++)
				{	
					sembuffer.sem_num = 0;
					sembuffer.sem_op = -1;
					sembuffer.sem_flg = SEM_UNDO;
					semop(mapid, &sembuffer, 1);
					if(filenode[j] == '0')
					{
						FILE *fp = fopen("./real", "r+");
						filenode[j] = '1';
						
						sembuffer.sem_op = 1;	
						semop(mapid, &sembuffer, 1);
						write_filenode();			
printf("%d::%d\n", j + 1, i);
						fcbs[curdir->inode].ext[k] = j + 1;
						fseek(fp, j * 512, SEEK_CUR);
						putw(i, fp);
fseek(fp, -4, SEEK_CUR);
printf("write : %d\n", getw(fp));
fseek(fp, 4, SEEK_CUR);
						putw(0, fp);
						free(fp);
						break ;
					}	
					sembuffer.sem_op = 1;	
					semop(mapid, &sembuffer, 1);
				}
			}
			else
			{
				int temp = fcbs[curdir->inode].ext[k];
				FILE *fp = fopen("./real", "r+");
				int retval = 0;
				int count = 0;
				fseek(fp, (temp - 1) * 512, SEEK_CUR);	
				retval = getw(fp);
				while(retval != 0 && retval != -1)
				{
					count++;
					retval = getw(fp);
				}
				if(count == 127)
				{
					free(fp);
					message_send("the file in current directory reach toplimit\n");
					return ;
				}
				fseek(fp, -4, SEEK_CUR);
				putw(i, fp);
fseek(fp, -4, SEEK_CUR);
printf("write : %d\n", getw(fp));
fseek(fp, 4, SEEK_CUR);
				putw(0, fp);	
				free(fp);	
			}
		}
		else if(fcbs[curdir->inode].ext[k] == 0)
		{
			fcbs[curdir->inode].ext[k] = i;
			break;
		}
	}
	sembuffer.sem_op = 1;	
	semop(semid, &sembuffer, 1);
	message_send("");
	return ;
}



void touch(char *param1, char *param2)
{
	mkdir(param1, param2, 'f');
}



void cd(char *f_name)
{
	struct directory *p = &dirchain;
	if(!strcmp(f_name, ".."))
	{
		if(strcmp(curdir->d_name, "/"))
		{
			while(p->next->next != NULL)
			{
				p = p->next;
			}
			p->next = NULL;
			curdir = p;
			message_send("");
		}
		else
		{
			message_send("cd:you are in the root dir!\n");
		}
		return ;
	}
	int index = get_inode_by_name(f_name);
	if(index == -1)
	{
		message_send("cd:directory did not find!\n");
		return ;
	}
	if(fcbs[index].f_type != 'd')
	{
		memset(buf, 0, sizeof(buf));
		message_send(strcat(strcat(strcat(buf, "cd:"), f_name), " is not a directory!\n"));		
		return ;
	}
	if(auth('x', &fcbs[index]))
	{
		struct directory *b;
		b = getpch(struct directory);
		strcpy(b->d_name, f_name);
		b->inode = fcbs[index].inode;
		b->next = NULL;
		curdir->next = b;
		curdir = curdir->next;
		message_send("");
	}
	else
	{
		message_send("cd:you have no access to change directory into this dir!\n");
	}
	return ;
}


void rm_dir(char *d_name)
{
	int k;
	int index = get_inode_by_name(d_name);
	if(index == -1)
	{
		message_send("rmdir:directory did not find!\n");
		return ;
	}
	if(fcbs[index].f_type == 'f')
	{
		memset(buf, 0, sizeof(buf));
		message_send(strcat(strcat(strcat(buf, "rmdir:"), d_name), " is a file!\n"));
		return ;
	}	
	if(auth('x', &fcbs[index]))
	{
		for(k = 0; k < getsize(fcbs[index].ext); k++) 
		{
			if(fcbs[index].ext[k] != 0)
			{
				memset(buf, 0, sizeof(buf));
				message_send(strcat(strcat(strcat(buf, "rmdir:"), d_name), " is not empty!\n"));
				return ;
			}
		}
		fcbs[index].flag = 0;
		indexs[index].flag = 0;
		for(k = 0; k < getsize(fcbs[index].ext); k++)
		{
			if(fcbs[curdir->inode].ext[k] == index)
			{
				fcbs[curdir->inode].ext[k] = 0;
				message_send("");
				return ;
			}
		}			
	}
	else
	{
		message_send("rmdir:you have no access to remove this directory!\n");
		return ;		
	}	
}


void rm_file(char *f_name)
{
	int k;
	int index = get_inode_by_name(f_name);
	if(index == -1)
	{
		message_send("rm:file did not find!\n");
		return ;
	}
	if(fcbs[index].f_type == 'd')
	{
		memset(buf, 0, sizeof(buf));
		message_send(strcat(strcat(strcat(buf, "rm:"), f_name), " is a directory!\n"));
		return ;			
	}
	if(auth('w', &fcbs[index]))
	{
		fcbs[index].flag = 0;
		indexs[index].flag = 0;
		for(k = 0; k < getsize(fcbs[index].ext); k++)
		{
			if(fcbs[curdir->inode].ext[k] == index)
			{
				fcbs[curdir->inode].ext[k] = 0;
				message_send("");
				return ;
			}
		}
	}
	else
	{
		message_send("rm:you have no access to write this directory!\n");
		return ;
	}
}


void useradd(char *u_name, char *g_name)
{
	int i, j;
	if(curuser.uid != 1)
	{
		message_send("you have no access to add a user!\n");
		return ;
	}
	if(!strcmp(u_name, "root"))
	{
		message_send("root can not be add\n");
		return ;
	}
	if(!strcmp(g_name, "root"))
	{
		message_send("root group root can not add any user\n");
		return ;
	}
	for(j = 0; j < GROUPLEN; j++)
	{
		if(!strcmp(groupnode[j].g_name, g_name))
		{
			break;
		}
	}
	if(j == GROUPLEN)
	{
		message_send("group no exist\n");
		return ;
	}
	for(i = 0; i < USERLEN; i++)
	{
		if(usernode[i].uid == 0)
		{
			break;
		}
		if(!strcmp(u_name, usernode[i].username))	
		{
			message_send("user already existed\n");
			return ;			
		}
	}
	usernode[i].uid = usernode[i == 0 ? 0 : i - 1].uid + 1;
	usernode[i].gid = groupnode[j].gid;
	memset(usernode[i].username, 0, sizeof(usernode[i].username));
	strcpy(usernode[i].username, u_name);
	memset(usernode[i].passwd, 0, sizeof(usernode[i].passwd));
	strcpy(usernode[i].passwd, "123456");
	write_user();
	return ;
}


void groupadd(char *g_name)
{
	int i;
	if(curuser.uid != 1)
	{
		message_send("you have no access to add a group!\n");
		return ;
	}
printf("%d\n", strlen(g_name));
	if(!strcmp(g_name, "root"))
	{
		message_send("root can not be add\n");
		return ;
	}
	for(i = 0; i < GROUPLEN; i++)
	{
		if(groupnode[i].gid == 0)
		{
			break;
		}
		if(!strcmp(g_name, groupnode[i].g_name))	
		{
			message_send("group already existed\n");
			return ;			
		}
	}
	if(i == GROUPLEN)
	{
		message_send("group full, can not add any more\n");
		return ;
	}
	groupnode[i].gid = groupnode[i == 0 ? 0 : i - 1].gid + 1;
	strcpy(groupnode[i].g_name, g_name);
	write_group();	
	return ;
}



void userrm(char *u_name)
{
	int i, j;
	if(curuser.uid != 1)
	{
		message_send("you have no access to remove a user!\n");
		return ;
	}
	if(!strcmp(u_name, "root"))
	{
		message_send("root can not be removed\n");
		return ;
	}
	for(i = 0; i < USERLEN; i++)
	{
		if(!strcmp(usernode[i].username, u_name))
		{
			break;
		}
	}
	if(i == USERLEN)
	{
		message_send("user no find\n");
		return ;
	}
	for(j = i + 1; j < USERLEN; j++)
	{
		usernode[j - 1].uid = usernode[j].uid;
		usernode[j - 1].gid = usernode[j].gid;
		memset(usernode[j - 1].username, 0, sizeof(usernode[j - 1].username));
		strcpy(usernode[j - 1].username, usernode[j].username);
		memset(usernode[j - 1].passwd, 0, sizeof(usernode[j -1].passwd));
		strcpy(usernode[j - 1].passwd, usernode[j].passwd);
	}	
	usernode[USERLEN - 1].uid = 0;
	usernode[USERLEN - 1].gid = 0;
	memset(usernode[USERLEN - 1].username, 0, sizeof(usernode[USERLEN - 1].username));
	memset(usernode[USERLEN - 1].passwd, 0, sizeof(usernode[USERLEN - 1].passwd));
	write_user();
	return ;	
}

void grouprm(char *g_name)
{
	int i, j;
	if(curuser.uid != 1)
	{
		message_send("you have no access to remove a group!\n");
		return ;
	}
	if(!strcmp(g_name, "root"))
	{
		message_send("root can not be removed\n");
		return ;
	}
	for(i = 0; i < GROUPLEN; i++)
	{
		if(!strcmp(g_name, groupnode[i].g_name))	
		{
			break;
		}
	}
	if(i == GROUPLEN)
	{
		message_send("group no find\n");
		return ;	
	}
	for(j = 0; j < USERLEN; j++)
	{
		if(usernode[j].gid == groupnode[i].gid)
		{
printf("%d\n", groupnode[i].gid);
			message_send("there is some user in this group, can not remove\n");
			return ;			
		}
	}
	for(j = i + 1; j < GROUPLEN; j++)
	{
		
		groupnode[j - 1].gid = groupnode[j].gid;
		memset(groupnode[j - 1].g_name, 0, sizeof(groupnode[j - 1].g_name));
		strcpy(groupnode[j - 1].g_name, groupnode[j].g_name);
	}
	groupnode[GROUPLEN -1].gid = 0;	
	memset(groupnode[GROUPLEN - 1].g_name, 0, sizeof(groupnode[GROUPLEN - 1].g_name)); 
	write_group();
				

	//message_send("");
	return;

}

void chmod(char *f_name, char *access)
{
	int i, j;
	for(i = 0; i < LEN; i++)
	{
		if(!strcmp(indexs[i].i_name, f_name))
		{
			break;
		}
	}
	if(i == LEN)
	{
		message_send("file no find\n");
		return ;
	}
	if(curuser.uid != 1 && curuser.uid != fcbs[i].uid)
	{
		message_send("you have no access to chmod!\n");
		return ;
	}
	memset(fcbs[i].access, 0, sizeof(fcbs[i].access));
	strcpy(fcbs[i].access, access);
	message_send("");
	return ;
}

