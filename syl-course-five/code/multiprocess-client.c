/*
    file: multiprocess-client.c
    build-cmd: gcc multiprocess-client.c socket-comm.c -o multiprocess-client
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket-comm.h"

#define MAXLINE 60
#define SERV_PORT 8080  // 连接到服务端的端口号


int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;

    // 创建 socket 套接字
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    // 地址结构信息赋值
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

    // 建立连接
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // 循环获取服务端发过来的数据
	while (fgets(buf, MAXLINE, stdin) != NULL) {
        // 发送数据到服务端
		Write(sockfd, buf, strlen(buf));
        // 读取服务端发过来的数据
		n = Read(sockfd, buf, MAXLINE);
		if (n == 0) {
			printf("the other side has been closed.\n");
			break;
		} else
            // 输出接收到的数据 
			Write(STDOUT_FILENO, buf, n);
	}
    // 关闭文件套接字
	Close(sockfd);
	return 0;
}
