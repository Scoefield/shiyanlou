#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define SERV_PORT 8899  // 宏定义服务器端口为：8899

// 主函数
int main(int argc, char *argv[])
{    
    struct sockaddr_in serv_addr;                // 定义服务器地址结构

    serv_addr.sin_family = AF_INET;              // 表示使用 IPv4 协议
    serv_addr.sin_port = htons(SERV_PORT);       // 将端口号转为网络字节序
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    int clientfd;
    clientfd = socket(AF_INET, SOCK_STREAM, 0);  // 创建客户端套接字
    if (clientfd == -1)                          // 套接字为-1 表示失败
    {
        perror("socket error!");                 // 打印错误信息
	    exit(1);                                 // 并退出
    }

    // 传入相关参数，连接到服务器
    int conn_ret= connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (conn_ret!= 0)
    {
        perror("connect error!");                 // 打印错误信息
	    exit(1);                                  // 并退出
    }

    char buf[BUFSIZ];
    int counter = 10;                               // 10 次计数，配合下面的每次睡眠 1 秒
    while (--counter) {                             // 则相当于发送 10 次数据，即打印 10 次 hello world
        write(clientfd, "hello world\n", 12);       // 发送 hello world 给服务器端
        conn_ret= read(clientfd, buf, sizeof(buf)); // 读取服务器端返回的数据
        write(STDOUT_FILENO, buf, conn_ret);        // 将读取到的内容写到标准输出
        sleep(1);                                   // 睡眠 1 秒
    }

    close(clientfd);                                // 关闭客户端文件描述符

	return 0;
}
