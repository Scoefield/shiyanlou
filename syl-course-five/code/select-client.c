/*
    file: select-client.c
    build-cmd: gcc select-client.c socket-comm.c  -o select-client
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "socket-comm.h"

#define MAXLINE 80		// buf 缓冲区大小
#define SERV_PORT 6666	// 端口号

// 客户端主函数（入口函数）
int main(int argc, char *argv[])
{
	// 套接字变量和buf变量声明
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;

	// 创建套接字
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	// 地址结构赋值及地址字节序转换
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	// 连接套接字
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// 获取数据
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		// 将 buf 数据的 sockfd
		Write(sockfd, buf, strlen(buf));
		// 读取服务端发送过来的数据
		n = Read(sockfd, buf, MAXLINE);
		if (n == 0)
			printf("the other side has been closed.\n");
		else
			// 输出到控制台
			Write(STDOUT_FILENO, buf, n);
	}
	// 关闭套接字
	Close(sockfd);
	return 0;
}
