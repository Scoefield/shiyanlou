/*
    file: epoll-server.c
    bulid-cmd: gcc epoll-server.c socket-comm.c  -o epoll-server
*/

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <unistd.h>

#include "socket-comm.h"

#define MAXLINE 80		// buf 缓冲区大小
#define SERV_PORT 6666	// 端口号
#define OPEN_MAX 1024	// 打开客户端最大值 1024

// 服务端主函数（入口函数）
int main(int argc, char *argv[])
{
	// 一些变量声明
	int i, j, maxi, listenfd, connfd, sockfd;
	int nready, efd, res;
	ssize_t n;
	char buf[MAXLINE], str[INET_ADDRSTRLEN];
	socklen_t clilen;
	int client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;
	struct epoll_event tep, ep[OPEN_MAX];

	// 1. 创建套接字
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	// 2. 绑定地址结构
	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	// 3. 监听客户端连接的到来，设置上限为 10
	Listen(listenfd, 10);

	// 初始化 client[] 集合
	for (i = 0; i < OPEN_MAX; i++)
		client[i] = -1;
	maxi = -1;

	// 创建 epoll 文件描述符
	efd = epoll_create(OPEN_MAX);
	if (efd == -1)
		perr_exit("epoll_create");

	// 设置监听事件和文件描述符
	tep.events = EPOLLIN;
	tep.data.fd = listenfd;

	// 设置 epoll 监控的文件描述符的事件，EPOLL_CTL_ADD
	res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);
	if (res == -1)
		perr_exit("epoll_ctl");

	while (1) {
		// 阻塞监听事件的到来
		nready = epoll_wait(efd, ep, OPEN_MAX, -1);
		if (nready == -1)
			perr_exit("epoll_wait");

		for (i = 0; i < nready; i++) {
			// 判断事件为非 EPOLLIN 可读事件时，跳过，否则往下执行
			if (!(ep[i].events & EPOLLIN))
				continue;
			// 处理监听到的事件
			if (ep[i].data.fd == listenfd) {
				clilen = sizeof(cliaddr);
				connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
				printf("received from %s at PORT %d\n", 
						inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), 
						ntohs(cliaddr.sin_port));
				for (j = 0; j < OPEN_MAX; j++) {
					if (client[j] < 0) {
						// 将响应的客户端文件描述符保存到 client[]
						client[j] = connfd;
						break;
					}
				}
				// 打开的文件描述符超过最大值，这里定义为 OPEN_MAX=1024
				if (j == OPEN_MAX)
					perr_exit("too many clients");
				// client[] 最大索引值赋值
				if (j > maxi)
					maxi = j;

				// 设置监听事件和文件描述符
				tep.events = EPOLLIN; 
				tep.data.fd = connfd;
				// 将新的 fd 注册到，epfd EPOLL_CTL_ADD
				res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
				if (res == -1)
					perr_exit("epoll_ctl");
			} else {
				sockfd = ep[i].data.fd;
				// 读取数据
				n = Read(sockfd, buf, MAXLINE);
				if (n == 0) {
					for (j = 0; j <= maxi; j++) {
						if (client[j] == sockfd) {
							client[j] = -1;
							break;
						}
					}
					// 从 epfd 删除一个 fd
					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
					if (res == -1)
						perr_exit("epoll_ctl");
					// 关闭 sockfd
					Close(sockfd);
					printf("client[%d] closed connection\n", j);
				} else {
					for (j = 0; j < n; j++)
						buf[j] = toupper(buf[j]);
					Writen(sockfd, buf, n);
				}
			}
		}
	}
	// 关闭套接字
	close(listenfd);
	close(efd);
	return 0;
}
