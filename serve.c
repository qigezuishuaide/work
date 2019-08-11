/*服务器*/
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#define SERV_PORT        9877 /*网络连接的端口号*/
#define LISTENQ     1024 /*listen函数的backlog*/
#define MAXLINE 		 4096 /*最大的数据读取量*/
int main (int argc, char** argv)
{
	int connfd, listenfd;
	int recvtmp = 0;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t clilen;

	//if (2 != argc)
	//{
	//	perror("usage:tcpcli<IPaddress>");
	//}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == listenfd)
	{
		perror("socket创建失败：");
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	recvtmp = bind(listenfd, (struct sockaddr*)&servaddr,sizeof(servaddr));
	if(0 != recvtmp)
	{
		perror("bind失败：");
	}

	recvtmp = listen(listenfd, LISTENQ);
	connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
	int n;
	char buf[MAXLINE];
	while((n = read(connfd, buf, MAXLINE)) > 0)
	{
		write(STDOUT_FILENO, buf, n);
	}
	return 0;
}
