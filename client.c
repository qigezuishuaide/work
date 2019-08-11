#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>
#include "client.h"

#define IP_ADDRESS 	1 	/*输入参数中第几个是IP地址*/
#define FILE_PATH 	2 	/*输入参数中第几个是文件路径*/
#define PATH_SIZE 	256	/*绝对路径的缓冲数组长度*/

int abs_read(char* file_path)//绝对路径的文件描述服
{
	int absfd = 0; //最终返回的文件描述符
	int pathfd = 0; //绝对路径文件描述符
	char pathbuf[PATH_SIZE];//当前绝对目录

	bzero(pathbuf, PATH_SIZE);
	getcwd(pathbuf, PATH_SIZE); //获取当前绝对目录
	pathfd = open(pathbuf, O_RDONLY); //获取当前目录文件描述符
	if(-1 == pathfd)
	{
		perror("路径获取失败");
		return 0;
	}

	absfd = openat(pathfd, file_path, O_RDONLY); //绝对路径和相对路径输入均可
	close(pathfd);
	return absfd;
}
void file_trans(int sockfd, char* file_path)
{
	char readbuf[READ_MAX];
	int readlen = 0;
	int readfd = 0;

	bzero(readbuf, sizeof(readbuf));

	readfd = abs_read(file_path);

	do
	{
		readlen = read(readfd, readbuf, READ_MAX);
		if(readlen < 0)
		{
			perror("文件读取失败");
		}
		else if(0 == readlen)
		{
			break;
		}
		else
		{
			write(sockfd, readbuf, READ_MAX);
		}
	}while(1);
	close(readfd);
	return ;
}

int socket_pre(char* argv)
{
	int sockfd = 0;
	int recvtmp = 0; //返回值临时变量
	struct sockaddr_in servaddr;
	if(NULL == argv)
	{
		printf("socket_pre参数传递错误\n");
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sockfd)
	{
		perror("socket创建失败:");
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	recvtmp = inet_pton(AF_INET, argv, &servaddr.sin_addr);
	if(1 != recvtmp)
	{
		perror("网络地址转换失败:");
	}
	recvtmp = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if(0 != recvtmp)
	{
		perror("网络连接失败:");
	}

	return sockfd;
}

int main (int argc, char** argv)
{
	int sockfd = 0; //传输文件描述符

	if (3 != argc)
	{
		perror("usage:tcpcli <ipaddress>");
	}

	sockfd = socket_pre(argv[IP_ADDRESS]);

	file_trans(sockfd, argv[FILE_PATH]);
	return 0;
}
