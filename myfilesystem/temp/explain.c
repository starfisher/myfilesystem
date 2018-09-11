#include "filesym.h"

char op_arr[OPLEN][LEN];
extern struct directory *curdir;
extern struct user curuser;
extern struct index *indexs;//[LEN];
extern struct command commandchain;

char buf[1024];


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
			message_send("you input ls....\n");
		}
		else if(!strcmp("ll", op_arr[0]))
		{
			message_send("you input ll....\n");
		}
		else if(!strcmp("open", op_arr[0]))
		{
			message_send("you input open....\n");
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
			message_send("you input pwd....\n");
		}
		else if(!strcmp("logout", op_arr[0]))
		{
			message_send("you input logout....\n");
		}
		else if(!strcmp("exit", op_arr[0]))
		{
			message_send("you input exit....\n");
		}
		else if(!strcmp("useradd", op_arr[0]))
		{
			message_send("you input useradd....\n");
		}
		else if(!strcmp("userrm", op_arr[0]))
		{
			message_send("you input userrm....\n");
		}
		else if(!strcmp("groupadd", op_arr[0]))
		{
			message_send("you input groupadd....\n");
		}
		else if(!strcmp("grouprm", op_arr[0]))
		{
			message_send("you input grouprm....\n");
		}
		else if(!strcmp("chmod", op_arr[0]))
		{
			message_send("you input chmod....\n");
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

