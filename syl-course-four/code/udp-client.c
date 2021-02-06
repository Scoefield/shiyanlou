/*
    file: udp-client.c
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define SERV_PORT 8000

int main(int argc, char *argv[])
{
    // 定义相关结构和缓冲变量
    struct sockaddr_in servaddr;
    int sockfd, n;
    char buf[BUFSIZ];

    // 同样客户端也要创建套接字（文件描述符），参数说同服务器端
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 清空地址结构，再对地址结构进行相关赋值
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    // 绑定通信地址，客户端的 bind() 可有可无，因为下面 sendto() 函数的参数 &servaddr 已指定了服务器的地址结构了
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while (fgets(buf, BUFSIZ, stdin) != NULL) {
        // 发送数据给服务端
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (n == -1)
            perror("sendto error");

        // 接收数据，参数NULL：表示不关心对端信息
        n = recvfrom(sockfd, buf, BUFSIZ, 0, NULL, 0);
        if (n == -1)
            perror("recvfrom error");

        write(STDOUT_FILENO, buf, n);
    }

    // 关闭文件描述符
    close(sockfd);

    return 0;
}
