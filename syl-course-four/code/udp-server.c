/*
    file: udp-server.c
*/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define SERV_PORT 8000  // 定义服务端的端口号为 8000

int main(void)
{
    struct sockaddr_in serv_addr, clie_addr;    // 创建服务端地址和客户端地址的结构体变量
    socklen_t clie_addr_len;
    int sockfd;
    char buf[BUFSIZ];           // buf 缓冲区，最大值为 BUFSIZ，系统默认设置 8192
    char str[INET_ADDRSTRLEN];  // IPV4 网络地址长度，INET_ADDRSTRLEN：16
    int i, n;

    // 创建套接字 socket，即文件描述符，AF_INET 参数表示IPV4，下面会解释说明
    // SOCK_STREAM 表示流式协议 TCP 用的，这里用 SOCK_DGRAM 表示报式协议 UDP 用
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 清空地址结构、再进行地址相关赋值和转换，比如：地址族用 AF_INET 表示IPV4，htons 字节序转换，字节序相关可以参见前面的实验说明
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);

    // 绑定服务器地址
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Accepting connections ...\n");
    while (1) {
        clie_addr_len = sizeof(clie_addr);
        // 在 TCP 是用 recv() 替换了原来使用的 read()，send()替换了write()，而在 UDP 是用 recvfrom() 和 sendto()
        // sockfd：套接字，buf：缓冲区地址，BUFSIZ：缓冲区大小，0：默认值，clie_addr：传出参数客户端（对端）地址结构，clie_addr_len：传出地址长度
        // 返回值：成功接收数据的字节数，失败：-1，0 的话表示对端关闭
        n = recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockaddr *)&clie_addr, &clie_addr_len);
        if (n == -1)
            perror("recvfrom error");

        // 打印接收客户端的信息
        printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &clie_addr.sin_addr, str, sizeof(str)), ntohs(clie_addr.sin_port));
        // 处理数据：小写转大写
        for (i = 0; i < n; i++)
            buf[i] = toupper(buf[i]);

        // 发送回给客户端
        // sockfd：套接字，buf：存储数据的缓冲区，n：数据长度，clie_addr：传入参数目标（对端）地址结构，sizeof(clie_addr)：地址结构长度
        // 返回值：成功写出的字节数，失败：-1
        n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&clie_addr, sizeof(clie_addr));
        if (n == -1)
            perror("sendto error");
    }

    // 最后关闭文件描述符
    close(sockfd);

    return 0;
}
