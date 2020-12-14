/*
    file: server.c
    desc: 服务器端的作用是从客户端读取字符数据，然后将每个字符转为大写并发回给客户端。
*/

#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define SERV_PORT 8899  // 宏定义服务器端口为：8899

// 主函数
int main(int argc, char *argv[])
{
    int listenfd = 0, coonfd = 0;                       // 定义用于套接字连接变量
    int read_ret, i;                                 
    char buf[BUFSIZ], client_IP[1024];                  

    struct sockaddr_in server_addr, client_addr;        // 定义服务器和客户端地址结构：server_addr，client_addr
    socklen_t client_addr_len;				            // 客户端地址结构大小

    server_addr.sin_family = AF_INET;				    // 表示使用 IPv4 协议
    server_addr.sin_port = htons(SERV_PORT);		    // 将端口号转为网络字节序
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 获取本机的任意有效IP，并转网络字节序

    listenfd = socket(AF_INET, SOCK_STREAM, 0);		    // 创建一个套接字 socket
    if (listenfd == -1) {                               // listenfd 为 -1 表示创建失败
        perror("socket error!");                        // 打印错误信息
	    exit(1);                                        // 并退出
    }

    // 给服务器 socket 绑定地址结构（IP+port)
    bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(listenfd, 256);					            // 设置监听上限，这里设置为 256

    client_addr_len = sizeof(client_addr);	            // 获取客户端地址结构大小

    // 调用 accept() 阻塞等待客户端连接请求，返回 coonfd 为-1 的话，表示失败
    coonfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (coonfd == -1)
    {
        perror("accept error!");                        // 打印错误信息
	    exit(1);                                        // 并退出
    }

    // 请求成功后，打印连接进来的客户端信息：根据 accept 传出参数，获取客户端 ip 地址 和 port 端口号
    printf("client ip:%s port:%d\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, 
            client_IP, sizeof(client_IP)), ntohs(client_addr.sin_port));			

    // 循环处理数据
    while (1) {
        read_ret = read(coonfd, buf, sizeof(buf));		// 读客户端发过来的数据
        write(STDOUT_FILENO, buf, read_ret);			// 将读取到的数据写到标准输出

        for (i = 0; i < read_ret; i++)				    
        {
            buf[i] = toupper(buf[i]);                   // 小写转大写逻辑处理
        }

        write(coonfd, buf, read_ret);					// 转大写后，写回给客户端
    }

    // 关闭套接字文件描述符
    close(listenfd);
    close(coonfd);

    return 0;
}
