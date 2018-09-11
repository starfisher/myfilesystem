#include "filesym.h"

struct user *usernode;//[USERLEN];
struct fcb *fcbs;//[LEN];
struct index *indexs;//[LEN];
int semid;
struct directory *curdir;
struct user curuser;
struct directory dirchain;
struct group *groupnode;//[GROUPLEN];
char *filenode;//[2049];
struct command commandchain;
char command_tab[COMMANDLEN][10] = {"ls", "ll", "open", "mkdir", "touch", "rm", "rmdir" "cd", "pwd", "logout", "exit", "useradd", "userrm", "groupadd", "grouprm", "chmod"};

int main()
{
//	signal(SIGINT, save_config);
	init();
	socket_build();
	running();
	return 0;	
}
