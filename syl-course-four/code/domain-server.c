/*
    file: domain-server.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#define QLEN 10
/*
* 创建服务端套接字
* 创建成功则返回文件描述符，否则返回小于0的数
*/
int serv_listen(const char *name)
{
	int fd, len, err, rval;
	struct sockaddr_un un;

	/* 创建本地套接字流 */
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-1);
	/* 已存在时释放 */
	unlink(name);

	/* 赋值地址结构 */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);

	/* 绑定文件描述符 */
	if (bind(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -2;
		goto errout;
	}
    /* 监听文件描述符 */
	if (listen(fd, QLEN) < 0) {
		rval = -3;
		goto errout;
	}
    /* 返回文件描述符 */
	return(fd);

    errout:
        err = errno;
        close(fd);
        errno = err;
        return(rval);
}

// 服务端接收连接的建立
int serv_accept(int listenfd, uid_t *uidptr)
{
	int clifd, len, err, rval;
	time_t staletime;
	struct sockaddr_un un;
	struct stat statbuf;

	len = sizeof(un);
    // 接收连接
	if ((clifd = accept(listenfd, (struct sockaddr *)&un, &len)) < 0)
		return(-1);

	/* 从客户端的调用地址获取客户端的 uid */
	len -= offsetof(struct sockaddr_un, sun_path);
	un.sun_path[len] = 0;

    // 判断 socket 状态
	if (stat(un.sun_path, &statbuf) < 0) {
		rval = -2;
		goto errout;
	}
	if (S_ISSOCK(statbuf.st_mode) == 0) {
		rval = -3;
		goto errout;
	}
	if (uidptr != NULL)
		*uidptr = statbuf.st_uid;
	unlink(un.sun_path);
    // 返回接收到的客户端文件描述符
	return(clifd);

    // 报错输出错误码并关闭文件描述符
    errout:
        err = errno;
        close(clifd);
        errno = err;
        return(rval);
}

// 主函数
int main(void)
{
    // 初始化需要使用到的变量
	int lfd, cfd, n, i;
	uid_t cuid;
	char buf[1024];
    // 通过文件名 mylocal.socket 创建服务
	lfd = serv_listen("mylocal.socket");

    // 文件描述符 fd 小于0 返回相应报错信息
	if (lfd < 0) {
		switch (lfd) {
			case -3:perror("listen"); break;
			case -2:perror("bind"); break;
			case -1:perror("socket"); break;
		}
		exit(-1);
	}
    // 接收到了连接
	cfd = serv_accept(lfd, &cuid);
    // 如果 cfd 小于 0，输出相应的报错信息
	if (cfd < 0) {
		switch (cfd) {
			case -3:perror("非本地套接字"); break;
			case -2:perror("文件名有误"); break;
			case -1:perror("连接失败"); break;
		}
        // 退出
		exit(-1);
	}
    // 循环读取数据，并转大写（如果输入的是中文，返回的还是原句），再回写给客户端
	while (1) {
        r_again:
            // 读取客户端发送过来的数据并写到 buf
            n = read(cfd, buf, 1024);
            if (n == -1) {
                if (errno == EINTR)
                goto r_again;
            }
            else if (n == 0) {
                printf("对端已经关闭.\n");
                break;
            }
            // 在终端输出本地客户端发送过来的数据
            write(STDOUT_FILENO, buf, n);
            // 字符串转大写
            for (i = 0; i < n; i++)
            {
                buf[i] = toupper(buf[i]);
            }
            // 回写给客户端
            write(cfd, buf, n);
	}
    // 结束后，关闭客户端和服务端的文件描述符
	close(cfd);
	close(lfd);
	return 0;
}
