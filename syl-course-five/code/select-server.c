/*
    file: select-server.c
    build-cmd: gcc select-server.c socket-comm.c  -o select-server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#include "socket-comm.h"

#define MAXLINE 80		// buf 缓冲区大小
#define SERV_PORT 6666	// 端口号

// 主函数（入口函数）
int main(int argc, char *argv[])
{
	// 一些变量声明
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE]; 	// FD_SETSIZE 默认大小为 1024
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN]; 			// INET_ADDRSTRLEN 默认为 16
	socklen_t cliaddr_len;
	struct sockaddr_in cliaddr, servaddr;

	// 1. 创建套接字
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	// 地址结构赋值，以及地址字节序转换
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	// 2. 绑定地址结构
	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// 3. 监听套接字，这里设置监听上限10个，默认值为128
	Listen(listenfd, 10);

	// 初始化最大套接字，以及下标
	maxfd = listenfd;
	maxi = -1;

	for (i = 0; i < FD_SETSIZE; i++)
		// 用-1初始化client[]
		client[i] = -1;

	// 初始化 allset 地址
	FD_ZERO(&allset);
	// 构造 select 监控文件描述符集
	FD_SET(listenfd, &allset);

	// 循环
	for ( ; ; ) {
		// 新设置select监控信号集
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if (nready < 0)
			perr_exit("select error");
		
		// 有新的客户端连接进来
		if (FD_ISSET(listenfd, &rset)) {
			cliaddr_len = sizeof(cliaddr);
			connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
			// 打印客户端信息
			printf("received from %s at PORT %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
					ntohs(cliaddr.sin_port));
			for (i = 0; i < FD_SETSIZE; i++) {
				if (client[i] < 0) {
					// 保存accept返回的文件描述符到client[]里
					client[i] = connfd;
					break;
				}
			}
			// 判断是否达到达到 select 能监控的文件个数上限，默认 1024
			if (i == FD_SETSIZE) {
				fputs("too many clients\n", stderr);
				exit(1);
			}

			// 添加客户端的文件描述符到监控信号集里
			FD_SET(connfd, &allset);
			// 如果 connfd 大于 maxfd，将 connfd 赋值给 maxfd
			if (connfd > maxfd)
				maxfd = connfd;
			// 更新 client[]最大下标值为 i
			if (i > maxi)
				maxi = i;
			// 就绪文件描述符为0了，则继续回到上面 select 阻塞监听
			if (--nready == 0)
				continue;
		}

		// 检测有数据就绪的 client
		for (i = 0; i <= maxi; i++) {
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				// 客户端关闭连接时，服务器端也关闭对应连接
				if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
					Close(sockfd);
					// 清除 select 监控的文件描述符
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else {
					int j;
					// 转大写，写回到 sockfd
					for (j = 0; j < n; j++)
						buf[j] = toupper(buf[j]);
					Write(sockfd, buf, n);
				}
				if (--nready == 0)
					break;
			}
		}
	}
	close(listenfd);
	return 0;
}
