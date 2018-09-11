/*
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
*/

#include "filesym.h"

void send_command();
int getch();

//extern struct command commandchain;
char *host_name = "127.0.0.1";
//char *host_name = "192.168.6.9";  //local host
//char *host_name = "www.ifl.com";

int port = 8000;
char buf[1024];
char str[100];
int socket_descriptor;

int main(int argc, char *argv[])
{
	struct sockaddr_in pin;
//	struct hostent *server_host_name;
	
	bzero(&pin, sizeof(pin));
	pin.sin_family = AF_INET;
	inet_pton(AF_INET, host_name, &pin.sin_addr);
	pin.sin_port = htons(port);	
	if((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Error opening socket\n");
		exit(1);
	}
	if (connect(socket_descriptor, (struct sockaddr *)&pin, sizeof(pin)) == -1) 
	{
		perror("Error connecting to socket\n");
		exit(1);
	}
	printf("welcome to hetao's filesystem, please input 'login' or 'exit'..\n");
	memset(str, 0, sizeof(str));
	scanf("%s",str);
	if(!strcmp(str, "login"))
	{
		int i;
		printf("Username:");
		memset(str, 0, sizeof(str));
		scanf("%s",str);
		write(socket_descriptor, str, strlen(str)+1);
		getchar();
		printf("Password:");
		i = 0;
		while(1)
		{	
			str[i] = getch();
			if(str[i] == '\n' || i == sizeof(str))
			{
				str[i] = '\0';
				break;
			}
			if(str[i] == 127)
			{
				if(i != 0)
				{
					printf("\b \b");
					str[i--] = '\0';
				}
				continue;
			}
			i++;			
		}
		write(socket_descriptor, str, strlen(str)+1);
		read(socket_descriptor, buf, 1024);
		if(strcmp(buf, "login failed"))
		{
			int i, j;
			system("clear");
			printf("%s", buf);
			while(1)
			{
				send_command();
				for(i = 0; i < strlen(str); i++)
				{
					if(str[i] != ' ' && str[i] != '\t')
					{
						break;
					}
				}
				for(j = 0; j < strlen(str + i); j++)
				{
					if(str[j + i] == ' ' || str[j + i] == '\t' || str[j + i] == '\n')
					{
						break;
					}
				}	
				str[j + i] = '\0';
				if(!strcmp(str + i, "open"))
				{
					memset(buf, 0, sizeof(buf)); 
					read(socket_descriptor, buf, 1024);
					if(!strcmp(buf, "open: file can not find\n") || !strcmp(buf, "open: this file is a directory\n") 
						|| !strcmp(buf, "open: file can not open\n") || !strcmp(buf, "open: file is opened, can not open again\n"))
					{
						printf("%s", buf);	
						memset(buf, 0, sizeof(buf));
						read(socket_descriptor, buf, 1024);
						printf("%s", buf);		
					}
					else
					{
						int end = 0;
						char filebuf[5120];
						printf("%s", buf);
						while(1)
						{
							send_command();
							memset(filebuf, 0, sizeof(filebuf));
							read(socket_descriptor, filebuf, 5120);
							if(strcmp(filebuf, "close succeed\n"))
							{
								printf("%s", filebuf);
							}
							else
							{
								end = 1;
							}
							memset(buf, 0, sizeof(buf));
							read(socket_descriptor, buf, 1024);
							printf("%s", buf);
							if(end) break;
						}	
					}
					
				}
				else 
				{
					memset(buf, 0, sizeof(buf));
					read(socket_descriptor, buf, 1024);
					printf("%s", buf);	
					if(!strcmp(buf, "byebye\n"))
					{
						return ;
					}	
					memset(buf, 0, sizeof(buf));
					read(socket_descriptor, buf, 1024);
					printf("%s", buf);		
				}
			}			

		}
		else
		{
			printf("%s\n", buf);
		}
				
	

	}
	else if(!strcmp(str, "exit"))
	{
		exit(0);
	}
	else
	{
		printf("unknow command!\n");
	}

	close(socket_descriptor);
	return 1;

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


void send_command()
{
//	struct command *p = &commandchain;
	memset(str, 0, 100);
	int a = 0, flag = 0;	
	char c;
	while(1)
	{
		c = getch();
		if(flag && c != 9)
		{
		//	flag = 0;
		}
		if(c == 10)
		{
			str[a] = 10;
			break;
		}
		else if(c == 127)
		{
			if(a == 0)
			{
				continue;
			}
			str[--a] = '\0';
			printf("\b \b");
		}
		else if(c == 27)
		{
		/*	getch();
			c = getch();
			if(c == 65)
			{
				if(p->)		
			}	
			else if(c == 66)
			{
	
			}		
			continue;*/
		}
		else if(c == 9)
		{
	
		}
		else
		{
			str[a++] = c;
			printf("%c", c);
		}
	}
	printf("\n");
/*	if(str[0] != 10) 
	{
		struct command *new = getpch(struct command);
		p = &commandchain;
		
		strncpy(new->str, str, strlen(str) - 1);
		new->pre = p;
		new->next = p->next;
		if(p->next != NULL)
		{
			p->next->pre = new;
		}
		p->next = new;
	}*/
	write(socket_descriptor, str, strlen(str)+1);
}





/*
	if((server_host_name = gethostbyname(host_name)) == NULL) { 
		perror("Error resolving local host\n");
		exit(1);
	}
*/
//	bzero(&pin, sizeof(pin));
//	pin.sin_family = AF_INET;
//	//pin.sin_addr.s_addr = ((struct in_addr *)(server_host_name->h_addr))->s_addr;
//	inet_pton(AF_INET, host_name, &pin.sin_addr);
//	pin.sin_port = htons(port);
//	if((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//		perror("Error opening socket\n");
//		exit(1);
//	}
//	if (connect(socket_descriptor, (struct sockaddr *)&pin, sizeof(pin)) == -1) {
//		perror("Error connecting to socket\n");
//		exit(1);
//	}
	
//	printf("Sending message %s to server...\n", str);

//	if (send(socket_descriptor, str, strlen(str)+1, 0) == -1) {
//		perror("Error in send\n");
//		exit(1);
//	}

//	if (write(socket_descriptor, str, strlen(str)+1) == -1) {
//		perror("Error in send\n");
//		exit(1);
//	}
//	printf(".. sent message.. wait for response...\n");
	//close(socket_descriptor);
	//exit(1);
/*
	if(recv(socket_descriptor, buf, 8192, 0) == -1) {
		perror("Error in receiving response from server\n");
		exit(1);
	}
*/
//	if(read(socket_descriptor, buf, 8192) == -1) {
//		perror("Error in receiving response from server\n");
//		exit(1);
//	}
//	printf("\nResponse from server:\n\n%s\n", buf);
//	close(socket_descriptor);
//	return 1;
