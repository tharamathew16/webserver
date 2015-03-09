#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include <arpa/inet.h>

void http_head(int client);
void load_page(int client,FILE *);
void *thread_function(int );
int socket_create(char *,int);

int main(int argc,char *argv[])
{
	int sockfd,clientfd;
	pthread_t thread;
	struct sockaddr_in client_address;

//CREATE SOCKET//
	sockfd=socket_create(argv[1],argc);

	socklen_t client_len;
	client_len=sizeof(struct sockaddr_in);
	while(1)
	{
		clientfd=accept(sockfd,(struct sockaddr *)&client_address,&client_len);
		if(clientfd==-1)
		{
			perror("\n connection failed\n");
			exit(1);
		}
		else
		{
			printf("connection accepted\n");
		}

		int res;
		res=pthread_create(&thread,NULL,thread_function,(void *)clientfd);
		if(res!=0)
		{
			perror("thread creation failed\n");
			exit(EXIT_FAILURE);
		}
	}
	close(sockfd);
}

		
void *thread_function(int client)
{
	char buffer[BUFSIZ];
	memset(buffer,0,BUFSIZ);

	FILE *p;
		p=fopen("index.html","r");
		http_head(client);
		load_page(client,p);	
}

void http_head(int client)
{
	char response[] = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n\r\n";
        write(client, response, sizeof(response));
}

void load_page(int client,FILE *p)
{
	char buffer[BUFSIZ];
	while(!feof(p))
	{
		fgets(buffer,BUFSIZ,p);
		write(client,buffer,strlen(buffer));
	}

	close(client);
}	

int socket_create(char *c,int arg)
{
	int sockfd,l,b;
	struct sockaddr_in server_address;

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		perror("\n socket creation failed\n");
		exit(1);
	}

	else
		printf("\nsocket successfully created\n");

	int port_no;
	if(arg==1)
		port_no=8080;
	else
		port_no=atoi(c);

	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(port_no);
	server_address.sin_addr.s_addr=inet_addr("127.0.0.1");
	
	int server_len;
	server_len=sizeof(server_address);
//BINDING LOCAL NAMES TO A SOCKET//
	b=bind(sockfd,(struct sockaddr *)&server_address,server_len);
	if(b==-1)
	{
		perror("\n bind failed\n");
		exit(1);
	}
	else
	{
		printf("bind done\n");
	}

//CREATE SOCKET QUEUE//
	l=listen(sockfd,5);
	if(l==-1)
	{
		perror("\n listen failed\n");
		exit(1);
	}
	else
	{
		printf("allow incoming connections\n");
	}
	
	return sockfd;
}
