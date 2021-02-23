/*
    file: multithread-server.c
    build-cmd: gcc multithread-server.c socket-comm.c -o multithread-server -pthread
*/

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "socket-comm.h"

#define MAXLINE 60      // 定义缓冲区最大长度，单位：字节
#define SERV_PORT 6666  // 服务端端口号

// 定义地址信息结构体
struct s_info {
	struct sockaddr_in cliaddr;
	int connfd;
};

// 真正处理数据的函数 working
void *working(void *arg)
{
    // 声明相关变量
	int n,i;
	struct s_info *ts = (struct s_info*)arg;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];

	// 创建线程前设置线程创建的属性
	pthread_detach(pthread_self());

	while (1) {
        // 读取客户端 connfd 传过来的数据，放到缓冲区 buf，最大长度为 MAXLINE
		n = Read(ts->connfd, buf, MAXLINE);
		if (n == 0) {
			printf("the other side has been closed.\n");
			break;
		}
        // 打印接收到的客户端地址信息和端口号
		printf("received from %s at PORT %d\n",
				inet_ntop(AF_INET, &(*ts).cliaddr.sin_addr, str, sizeof(str)),
				ntohs((*ts).cliaddr.sin_port));
        // 数据转大写
		for (i = 0; i < n; i++)
			buf[i] = toupper(buf[i]);
        // 写回给客户端 connfd
		Write(ts->connfd, buf, n);
	}
    // 结束时，关闭 connfd 文件描述符
	Close(ts->connfd);
}

// 主函数（入口函数）
int main(void)
{
    // 地址结构变量声明
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
	int listenfd, connfd;
	int i = 0;
	pthread_t tid;
	struct s_info ts[256];

    // 创建套接字
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

    // 绑定地址信息并监听，设置监听上限为 20
	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	Listen(listenfd, 20);

	printf("Accepting connections ...\n");
	while (1) {
		cliaddr_len = sizeof(cliaddr);
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
		ts[i].cliaddr = cliaddr;
		ts[i].connfd = connfd;
		
        // 当达到线程最大值时，会有出错处理, 增加服务器的可用性
		pthread_create(&tid, NULL, do_work, (void*)&ts[i]);
		i++;
	}
	return 0;
}
