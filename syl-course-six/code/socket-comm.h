/*
    file: socket-comm.h
*/

#ifndef __WRAP_H_
#define __WRAP_H_

/* 在头文件声明相关封装函数 */

void perr_exit(const char *s);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int Bind(int fd, const struct sockaddr *sa, socklen_t salen);
int Connect(int fd, const struct sockaddr *sa, socklen_t salen);
int Listen(int fd, int backlog);
int Socket(int family, int type, int protocol);
ssize_t Read(int fd, void *ptr, size_t nbytes);
ssize_t Write(int fd, const void *ptr, size_t nbytes);
ssize_t Writen(int fd, const void *vptr, size_t n);
int Close(int fd);

#endif
