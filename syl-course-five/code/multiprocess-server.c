/*
    file: multiprocess-server.c
    build-cmd: gcc multiprocess-server.c socket-comm.c -o multiprocess-server
*/

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#include "socket-comm.h"

#define MAXLINE 60      // 定义缓冲区长度
#define SERV_PORT 8080  // 定义端口号

// 信号捕捉函数，完成子进程回收
void do_sigchild(int num)
{
    // 回收子进程用 waitpid，参数 0：同一个进程组，NULL：不关心状态，WNOHANG：不挂起（非阻塞）
	while (waitpid(0, NULL, WNOHANG) > 0);
}

// 主函数（入口函数）
int main(void)
{
	// 变量的声明
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
	int listenfd, connfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	int i, n;
	pid_t pid;

    // 信号捕捉处理
	struct sigaction newact;
	newact.sa_handler = do_sigchild;
	sigemptyset(&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction(SIGCHLD, &newact, NULL);

    // 创建并监听套接字，返回 lfd 文件描述符
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

    // 绑定地址结构
	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // 设置监听的上限，此处为 10
	Listen(listenfd, 10);

	printf("Accepting connections ...\n");
	while (1) {
		cliaddr_len = sizeof(cliaddr);
        // 接收客户端连接请求
		connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

        // fork 创建子进程，pid=0
		pid = fork();
		if (pid == 0) {
            // 创建子进程后，关闭用于建立连接的套接字 lfd
			Close(listenfd);
			while (1) {
                // 从客户端读取数据
				n = Read(connfd, buf, MAXLINE);
				if (n == 0) {
					printf("the other side has been closed.\n");
					break;
				}
                // 打印接收到客户端的地址信息，IP地址和端口号
				printf("received from %s at PORT %d\n",
						inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
						ntohs(cliaddr.sin_port));
				for (i = 0; i < n; i++)
					buf[i] = toupper(buf[i]);   // 小写转大写
				Write(connfd, buf, n);          // 写回给客户端
			}
			Close(connfd);
			return 0;
		} else if (pid > 0) {
			Close(connfd);      // 关闭用于与客户端通信的套接字 cfd
		} else
			perr_exit("fork");  // fork 创建子进程有误
	}
	Close(listenfd);            // 关闭套接字
	return 0;
}
