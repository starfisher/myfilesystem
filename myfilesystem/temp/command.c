#include "filesym.h"

extern struct directory *curdir;
extern struct index *indexs;//[LEN];
extern struct user *usernode;//[USERLEN];
extern struct fcb *fcbs;//[LEN];
extern struct user curuser;
extern struct directory dirchain;
extern int semid;

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


void mkdir(char *param1, char *param2, char type)
{
	char name[10];
	char access[4];
	int flag = 0;
	int i, k, index = -1;
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
			fcbs[i].tm = get_cur_time();
			fcbs[i].flag = 1;
			break;
		}	
	}
	for(k = 0; k < sizeof(fcbs[curdir->inode].ext)/sizeof(fcbs[curdir->inode].ext[0]); k++)
	{
		if(fcbs[curdir->inode].ext[k] == 0)
		{
			fcbs[curdir->inode].ext[k] = i;
			break;
		}
	}
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










