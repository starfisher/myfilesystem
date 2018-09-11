#include "filesym.h"

char op_arr[OPLEN][LEN];
extern struct directory *curdir;
extern struct fcb *fcbs;
extern struct user curuser;
extern struct index *indexs;//[LEN];
extern struct command commandchain;

char buf[1024];

extern int semid;
struct sembuf sembuffer;

void get_param()
{
	int i, j = 0, k = 0;
	memset(op_arr, 0, sizeof(op_arr));
	for(i = 0; buf[i] != 0; i++)
	{
		if(buf[i] == 32 || buf[i] == 10)
		{
			if(buf[i - 1] != 32 && i != 0)
			{
				strncpy(op_arr[k], &buf[j], i - j);
				k++;
			}
			j = i + 1;
		}
		else
		{
			if(k == OPLEN)
			{
				break;
			}
		}
	}

}





void file_start(struct fcb *file)
{
	int i = 0, j, k;
	char temp[15];
	memset(temp, 0, sizeof(temp));
	strcpy(temp, indexs[file->inode].i_name);
	temp[strlen(temp)] = '>';

	while(1)
	{
		message_send(temp);
		memset(buf, 0, sizeof(buf));
		message_recv(buf, sizeof(buf));
		buf[strlen(buf) - 1] = '\0';	
		for(; i < strlen(buf); i++)
		{	
			if(buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n')
			{
				continue;
			}
			else break;
		}
		for(j = 0; j < strlen(buf + i); j++)
		{
			if(buf[j + i] == ' ' || buf[j + i] == '\t' || buf[j + i] == '\n')
			{
				break;
			}
		}
		j = j + i;
		buf[j] = '\0';
		for(k = 0; k < strlen(buf + j + 1); k++)
		{
			if(buf[k + j + 1] == ' ' || buf[k + j + 1] == '\t' || buf[k + j + 1] == '\n')
			{
				continue;
			}
			else break;
		}
		k = k + j + 1;
		if(!strcmp(buf + i, ""))
		{
			message_send("");
		}
		else if(!strcmp(buf + i, "read"))
		{
			if(auth('r', file))
			{
				read_block(file);
			}
			else
			{
				message_send("you have no access to read this file!\n");	
			}
		}
		else if(!strcmp(buf + i, "write"))
		{
			if(auth('w', file))
			{
				write_block(file, buf + k);
				message_send("write succeed\n");
			}
			else
			{
				message_send("you have no access to write this file!\n");
			}
		}
		else if(!strcmp(buf + i, "close"))
		{
			file = NULL;
			message_send("close succeed\n");
			return;
		}
		else
		{
			message_send("unknow command\n");
		}		
	}
	return ;
}








void start()
{
	memset(buf, 0, sizeof(buf));
	strcat(strcat(strcat(strcat(strcat(strcat(buf, "["), curuser.username), "@filesystem "), curdir->d_name), "]"), curuser.uid == 1 ? "#" : "$");
	message_send(buf);
	while(1)
	{
		
		message_recv(buf, sizeof(buf));
		get_param();
printf("%s:%d\n", op_arr[0], strlen(op_arr[0]));
		if(!strcmp("ls", op_arr[0]))
		{
			ls(0);
		}
		else if(!strcmp("ll", op_arr[0]))
		{
			ls(1);
		}
		else if(!strcmp("open", op_arr[0]))
		{
			int index = get_inode_by_name(op_arr[1]);
			if(index == -1)
			{
			//	memset(buf, 0, sizeof(buf));
			//	strcat(strcat(buf, op_arr[1]), " can not find!\n");
				message_send("open: file can not find\n");		
			}
			else if(fcbs[index].f_type == 'f')
			{
				
				sembuffer.sem_num = index;
				sembuffer.sem_op = -1;
				sembuffer.sem_flg = IPC_NOWAIT;
				if(semop(semid, &sembuffer, 1) == -1)
				{
					message_send("open: file is opened, can not open again\n");
					memset(buf, 0, sizeof(buf));
					strcat(strcat(strcat(strcat(strcat(strcat(buf, "["), curuser.username), "@filesystem "), curdir->d_name), "]"), curuser.uid == 1 ? "#" : "$");
					message_send(buf);
					continue ;
				}
				file_start(&fcbs[index]);
				
				sembuffer.sem_op = 1;	
				semop(semid, &sembuffer, 1);
			}
			else if(fcbs[index].f_type == 'd')
			{
			//	memset(buf, 0, sizeof(buf));
			//	strcat(strcat(buf, op_arr[1]), " is a directory!\n");
				message_send("open: this file is a directory\n");		
			}
			else message_send("open: file can not open\n");
		}
		else if(!strcmp("mkdir", op_arr[0]))
		{
			mkdir(op_arr[1], op_arr[2], 'd');	
		}
		else if(!strcmp("touch", op_arr[0]))
		{
			touch(op_arr[1], op_arr[2]);
		}
		else if(!strcmp("rm", op_arr[0]))
		{
			rm_file(op_arr[1]);			
		}
		else if(!strcmp("rmdir", op_arr[0]))
		{
			rm_dir(op_arr[1]);
		}
		else if(!strcmp("cd", op_arr[0]))
		{
			cd(op_arr[1]);
		}
		else if(!strcmp("pwd", op_arr[0]))
		{
			pwd();
		}
		else if(!strcmp("logout", op_arr[0]))
		{
			message_send("byebye\n");
			save_config();
			return ;
		}
		else if(!strcmp("exit", op_arr[0]))
		{
			message_send("byebye\n");
			save_config();
			return ;
		}
		else if(!strcmp("useradd", op_arr[0]))
		{
			useradd(op_arr[1], op_arr[2]);
		}
		else if(!strcmp("userrm", op_arr[0]))
		{
			userrm(op_arr[1]);
		}
		else if(!strcmp("groupadd", op_arr[0]))
		{
			groupadd(op_arr[1]);
		}
		else if(!strcmp("grouprm", op_arr[0]))
		{
			grouprm(op_arr[1]);
		}
		else if(!strcmp("chmod", op_arr[0]))
		{
			chmod(op_arr[1], op_arr[2]);			
		}
		else
		{
			message_send("unknow command...\n");		
		}
		//printf("%s\n", op_arr[0]); 
		memset(buf, 0, sizeof(buf));
		strcat(strcat(strcat(strcat(strcat(strcat(buf, "["), curuser.username), "@filesystem "), curdir->d_name), "]"), curuser.uid == 1 ? "#" : "$");
		message_send(buf);
	}

}

