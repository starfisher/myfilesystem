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
			system("clear");
			printf("%s", buf);
			while(1)
			{
				send_command();
				read(socket_descriptor, buf, 1024);
				printf("%s", buf);		
				memset(buf, 0, sizeof(buf));
				read(socket_descriptor, buf, 1024);
				printf("%s", buf);		
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
