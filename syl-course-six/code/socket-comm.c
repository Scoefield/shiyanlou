/*
	file: socket-comm.c
*/

#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

// 错误处理并退出函数
void perr_exit(const char *s)
{
	perror(s);
	exit(1);
}

// 封装创建套接字 socket 的函数
int Socket(int family, int type, int protocol)
{
	int n;
	n = socket(family, type, protocol);
	if ( n < 0)
	{
		perr_exit("socket error");
	}
	return n;
}

// 封装绑定地址信息的 bind 函数
int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	int n;
	n = bind(fd, sa, salen);
	if (n < 0)
	{
		perr_exit("bind error");
	}
	return n;
}

// 封装建立连接的 connect 函数
int Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	int n;
	n = connect(fd, sa, salen);
	if (n < 0)
	{
		perr_exit("connect error");
	}
	return n;
}

// 封装接收连接的 accept 函数
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;
	n = accept(fd, sa, salenptr);
	reAccept:
	if ( n < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR))
			goto reAccept;
		else
			perr_exit("accept error");
	}
	return n;
}

// 封装监听连接到来的 listen 函数
int Listen(int fd, int backlog)
{
	int n;
	n = listen(fd, backlog);
	if (n < 0)
	{
		perr_exit("listen error");
	}
	return n;
}

// 封装从文件描述符 fd 读取数据的 read 函数，返回读取到的字节数
ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	ssize_t n;
	n = read(fd, ptr, nbytes);
	readAgain:
	if ( n == -1) {
		if (errno == EINTR)
			goto readAgain;
		else
			return -1;
	}
	return n;
}

// 封装往文件描述符 fd 写数据的 write 函数，返回写入的字节数
ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
	ssize_t n;
	n = write(fd, ptr, nbytes);
	writeAgain:
	if ( n == -1) {
		if (errno == EINTR)
			goto writeAgain;
		else
			return -1;
	}
	return n;
}

// 封装关闭文件描述符（套接字）的 close 函数
int Close(int fd)
{
	int n;
	n = close(fd);
	if (n == -1)
		perr_exit("close error");
	return n;
}

// 封装往文件描述符 fd 写入 n 字节数据函数
ssize_t Writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;

	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}
