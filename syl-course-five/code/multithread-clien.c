/*
    file: multithread-client.c
    build-cmd: gcc multithread-client.c socket-comm.c -o multithread-client
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

#include "socket-comm.h"

#define MAXLINE 60
#define SERV_PORT 6666  // 服务端的端口号

// 主函数（入口函数）
int main(int argc, char *argv[])
{
    // 声明地址结构变量
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;

    // 创建套接字
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

    // 建立连接
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // 循环获取服务端发过来的数据
	while (fgets(buf, MAXLINE, stdin) != NULL) {
        // 往 sockfd 写数据，然后接收服务端写回的数据
		Write(sockfd, buf, strlen(buf));
		n = Read(sockfd, buf, MAXLINE);
		if (n == 0)
			printf("the other side has been closed.\n");
		else
            // 将接收到的数据打印输出
			Write(STDOUT_FILENO, buf, n);
	}
    // 结束后，关闭文件描述符 sockfd
	Close(sockfd);
	return 0;
}
