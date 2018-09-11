#include "filesym.h"

extern struct user *usernode;//[USERLEN];
extern struct group *groupnode;//[GROUPLEN];


void write_user()
{
	
	int i;
	char str[10];
	FILE *user = fopen("./user.ini", "w");
	if(user == 0)
	{
		message_send("user.ini write failed\n");
		return ;
	} 		
	for(i = 0; i < USERLEN; i++)
	{
		if(usernode[i].uid != 0)
		{	
			sprintf(str, "%d", usernode[i].uid);
			fputs(str, user);
			fputs(":", user);
			sprintf(str, "%d", usernode[i].gid);
			fputs(str, user);
			fputs(":", user);
			fputs(usernode[i].username, user);
			fputs(":", user);
			fputs(usernode[i].passwd, user);
			fputs("\n", user);
		}	
	}
	message_send("");
	return ;
}

void write_group()
{
	int i;
	char str[10];
	FILE *group = fopen("./group.ini", "w");
	if(group == 0)
	{
		message_send("group.ini write failed\n");
		return ;
	} 		
	for(i = 0; i < GROUPLEN; i++)
	{
		if(groupnode[i].gid != 0)
		{	
			sprintf(str, "%d", groupnode[i].gid);
			fputs(str, group);
			fputs(":", group);
			fputs(groupnode[i].g_name, group);
			fputs("\n", group);
		}	
	}
	message_send("");
	return ;
}
