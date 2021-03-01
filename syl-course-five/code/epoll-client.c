/*
    file: epoll-client.c
    bulid-cmd: gcc epoll-client.c socket-comm.c  -o epoll-client
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "socket-comm.h"

#define MAXLINE 80
#define SERV_PORT 6666	// 端口号

// 客户端主函数（入口函数）
int main(int argc, char *argv[])
{
	// 一些变量声明
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;

	// 创建套接字
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	// 连接套接字
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// 循环获取数据
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		// 写数据
		Write(sockfd, buf, strlen(buf));
		// 读数据
		n = Read(sockfd, buf, MAXLINE);
		if (n == 0)
			printf("the other side has been closed.\n");
		else
			// 在控制台打印数据数据
			Write(STDOUT_FILENO, buf, n);
	}
	// 关闭文件描述符
	Close(sockfd);
	return 0;
}