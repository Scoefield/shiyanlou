package main

import (
	"bufio"
	"fmt"
	"net"
)

// 处理数据函数
func process(conn net.Conn) {
	defer conn.Close() 			// 延迟关闭连接（函数执行完就关闭连接）
	for {
		reader := bufio.NewReader(conn)	// 创建带缓冲的读句柄
		var buf [128]byte
		n, err := reader.Read(buf[:])  // 读取数据

		// 如果有错，则打印报错信息，并退出 for 循环
		if err != nil {
			fmt.Println("read from client failed, err:", err)
			break
		}

		// 读取到的字节流数据转字符串，然后打印出来
		recvStr := string(buf[:n])
		fmt.Println("收到client端发来的数据：", recvStr)

		// 将收到的数据拼接 “from server” 字符串后发送回客户端
		conn.Write([]byte(recvStr + " from server"))
	}
}

// 主函数
func main() {
	// 绑定本地地址和端口号，使用 tcp 协议
	listen, err := net.Listen("tcp", "127.0.0.1:6666")
	// 如果有错，则打印报错信息
	if err != nil {
		fmt.Println("listen failed, err:", err)
		return
	}

	// for 循环监听请求到了
	for {
		conn, err := listen.Accept() // 建立连接
		if err != nil {
			fmt.Println("accept failed, err:", err)
			continue
		}
		process(conn) // 调用处理函数，执行处理数据逻辑
	}
}
