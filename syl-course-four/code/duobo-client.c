/*
    file: duobo-client.c
*/

#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>

#define SERVER_PORT 8888
#define CLIENT_PORT 9999
#define MAXLINE 4096        // 缓冲区 buf 大小
#define GROUP "239.0.0.6"   // 这里 GROUP 要与服务端的 GROUP 要对应

// 主函数
int main(int argc, char *argv[])
{
    // 声明相关变量
	struct sockaddr_in serveraddr, localaddr;
	int confd;
	ssize_t len;
	char buf[MAXLINE];

	// 定义组播结构体 创建套接字
	struct ip_mreqn group;
	confd = socket(AF_INET, SOCK_DGRAM, 0);

	// 初始化本地端地址
	bzero(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0" , &localaddr.sin_addr.s_addr);
	localaddr.sin_port = htons(CLIENT_PORT);

    // 绑定套接字
	bind(confd, (struct sockaddr *)&localaddr, sizeof(localaddr));

	// 设置组地址
	inet_pton(AF_INET, GROUP, &group.imr_multiaddr);
	// 本地任意IP
	inet_pton(AF_INET, "0.0.0.0", &group.imr_address);
	// 设置 client 加入多播组
	setsockopt(confd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));

	while (1) {
        // 接收数据写到 buf, 在终端打印 buf 接收到的数据
		len = recvfrom(confd, buf, sizeof(buf), 0, NULL, 0);
		write(STDOUT_FILENO, buf, len);
	}
    // 关闭套接字
	close(confd);
	return 0;
}
