#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <termios.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define getpch(type) (type*)malloc(sizeof(type))
#define getsize(arr) sizeof(arr)/sizeof(arr[0])

#define LEN 100 //数组长度
#define MAXBLOCK 2048 //最大的物理块数
#define BLOCKSIZE 512 //块大小
#define USERLEN 20  //用户数量
#define GROUPLEN 20 // 用户组数量
#define OPLEN 3 //指令长度
#define COMMANDLEN 16 //命令数目

typedef int BOOL;
#define FALSE 0
#define TRUE 1


struct command{
  char str[100];  //命令内容
  struct command *pre;
  struct command *next;
};

struct directory{
  int inode;
  char d_name[10];  //目录名
  struct directory *next;
};

struct group{
  int   gid;  //用户组id
  char  g_name[10];  //用户组名
};

struct user{
  int   gid;  //文件所属组id
  int   uid;  //文件主id
  char  passwd[8]; //密码
  char  username[10]; //用户名
};

struct index{ //索引目录
  int   inode; //inode节点号
  int   flag;
  char  i_name[10];  //文件名（最大10个字符）
};

struct fcb{ //文件控制块
  int   inode; //inode节点号
  int   uid; //文件主id
  int   gid; //文件所属组id
  int   f_size; //文件大小 单位B
  int   flag; //启用标志
  char  f_type;  //文件类型（f／d）d：目录，f：文件
  char  access[3];  //权限数组集 eg: 775
  int   ext[10]; //物理块号数组 文件最大为5120B
  struct tm tm; //修改时间
};

/******************************
        filerw.c
******************************/
void read_inode();
//void write_inode();
int read_file(FILE *fp);
void read_fcb();
//void write_fcb();
void read_user();
//void write_user();
void init();
//void save_config(int i);
void read_group();

/******************************
        fileblockrw.c
******************************/
void read_filenode();
void init_filenode();


/******************************
        tools.c
******************************/
int is_emtpy_file(char *f_name);
struct tm get_cur_time();
int getch();
int get_inode_by_name(char *name);
int auth(char op, struct fcb *file);

/******************************
	filesocket.c
******************************/
void socket_build();
void running();
void message_send(char *buf);
void message_recv(char *buf, int buf_len);


/******************************
	command.c
******************************/
BOOL login(char *username, char *passwd);
void mkdir(char *param1, char *param2, char type);
void touch(char *param1, char *param2);
void cd(char *f_name);
void rm_dir(char *f_name);
void rm_file(char *d_name);

/******************************
	explain.c
******************************/ 
void start();
void get_param();
