/*
    file: duobo-server.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8888
#define CLIENT_PORT 9999
#define MAXLINE 4096        // 缓冲区 buf 大小
#define GROUP "239.0.0.6"   // 多播组IP地址 224.0.0.0 到 239.255.255.255

int main(void)
{
    // 定义相关变量
	int sockfd, i ;
	struct sockaddr_in serveraddr, clientaddr;
	char buf[MAXLINE] = "This is the data sent by the server.\n";
	char ipstr[INET_ADDRSTRLEN];
	socklen_t clientlen;
	ssize_t len;
	struct ip_mreqn group;

	// 创建用于 UDP 通信的套接字
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 清空服务端地址结构、再进行地址相关赋值和转换，即初始化服务端地址
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;                // 表示 IPv4
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 表示本地任意IP INADDR_ANY = 0
	serveraddr.sin_port = htons(SERVER_PORT);

	// 构造客户端 client 地址结构 IP+端口
	bzero(&clientaddr, sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;        // 表示 IPv4
	inet_pton(AF_INET, GROUP, &clientaddr.sin_addr.s_addr);
	clientaddr.sin_port = htons(CLIENT_PORT);

	while (1) {
		// 每隔一秒，发送 buf 的数据出去
		sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
		sleep(1);
	}
    // 关闭 socket
	close(sockfd);
	return 0;
}
