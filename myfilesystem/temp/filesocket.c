#include "filesym.h"

int port = 8000;
struct sockaddr_in sockin, pin;
int sock_descriptor, temp_sock_descriptor;
socklen_t address_size;
int i, len, pid, on=1;


void sig_child(int signo)
{
	while(waitpid(-1, NULL, WNOHANG) > 0)
		;
	return;
}

void socket_build()
{
	signal(SIGCHLD, sig_child);
	sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_descriptor == -1)
	{
		perror("call to socket\n");
		exit(1);
	}
	bzero(&sockin, sizeof(sockin));
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = INADDR_ANY;
	sockin.sin_port = htons(port);
	//sin.sin_port = port;
	setsockopt(sock_descriptor,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	if(bind(sock_descriptor, (struct sockaddr*)&sockin, sizeof(sockin)) == -1) 
	{
		perror("call to bind");
		exit(1);
	}
	if(listen(sock_descriptor, 20) == -1) 
	{
		perror("call to listen");
		exit(1);
	}
	printf("Accepting connections ...\n");
	return ;	
}


void running()
{
	while(1)
	{	
		temp_sock_descriptor = accept(sock_descriptor, (struct sockaddr *)&pin, &address_size);
		if(temp_sock_descriptor == -1) 
		{
			perror("call to accept");
			exit(1);
		}
		if((pid=fork()) < 0)
		{
			perror("fork");
			exit(1);
		}
		else if(pid == 0) 
		{
			char username[10];
			char password[10];
			close(sock_descriptor);
			memset(username, 0, sizeof(username));
			if(recv(temp_sock_descriptor, username, sizeof(username), 0) == -1) 
			{
				perror("call to recv");
				exit(1);
			}
			memset(password, 0, sizeof(password));
			if(recv(temp_sock_descriptor, password, sizeof(password), 0) == -1) 
			{
				perror("call to recv");
				exit(1);
			}
			if(login(username, password))
			{
				start();
			}
			else
			{
				if(send(temp_sock_descriptor, "login failed", 14, 0) == -1) 
				{
					perror("call to send");
					exit(1);
				}
			}
			close(temp_sock_descriptor);
			exit(0);			
		}
		
		close(temp_sock_descriptor);
	}
}




void message_send(char *buf)
{
	if(send(temp_sock_descriptor, buf, strlen(buf) + 1, 0) == -1) 
	{
		perror("call to recv");
		exit(1);
	}
	return ;
}

void message_recv(char *buf, int buf_len)
{
	if(recv(temp_sock_descriptor, buf, buf_len, 0) == -1) 
	{
		perror("call to recv");
		exit(1);
	}
	return ;	
}



