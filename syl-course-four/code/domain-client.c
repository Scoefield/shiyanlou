/*
    file: domain-client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define CLI_PATH "/var/tmp/"    /* 创建 socket 时的 PID 路径 */
/*
* 创建客户端套接字并连接到服务器
* 创建成功则返回文件描述符，否则返回小于0的数
*/
int cli_conn(const char *name)
{
	int fd, len, err, rval;
	struct sockaddr_un un;

	/* 创建本地套接字 */
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-1);

	/* 用设置好的地址赋值套接字地址结构 */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	sprintf(un.sun_path, "%s%05d", CLI_PATH, getpid());
	len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

	/* 地址已经绑定，则返回报错信息 */
	unlink(un.sun_path);
	if (bind(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -2;
		goto errout;
	}

	/* 以服务器地址（文件名）创建连接 */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
	if (connect(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -4;
		goto errout;
	}
    return(fd);

    /* 报错输出，并关闭文件描述符 */
    errout:
        err = errno;
        close(fd);
        errno = err;
        return(rval);
}

// 主函数
int main(void)
{
	int fd, n;
	char buf[1024];

    // mylocal.socket：本地通信连接的名字
	fd = cli_conn("mylocal.socket");
    // fd 小于0 表示报错
	if (fd < 0) {
		switch (fd) {
			case -4:perror("connect"); break;
			case -3:perror("listen"); break;
			case -2:perror("bind"); break;
			case -1:perror("socket"); break;
		}
        // 退出
		exit(-1);
	}
    printf("请输入字符串，以回车结束：\n");
	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		write(fd, buf, strlen(buf));
		n = read(fd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, n);
	}
    // 关闭文件描述符
	close(fd);
	return 0;
}
