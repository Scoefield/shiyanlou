/*
	file: goroutine-server.go
	run-cmd: go run goroutine-server.go
*/

package main

import (
	"bytes"
	"fmt"
	"io"
	"net"
)

// 处理连接请求
func HandleConnectReq(conn net.Conn) {
	// defer 延迟关闭连接，即函数结束前关闭
	defer conn.Close()

	// 获取客户端的连接，返回连接的客户端地址信息
	addr := conn.RemoteAddr()
	// 打印连接成功的客户端地址信息
	fmt.Printf("客户端[%v]连接成功!\n", addr)

	// 循环读取客户端发送过来的数据
	buf := make([]byte, 1024)
	for {
		// read 读取数据放到buf，需要注意的是：read 会读取命令行的换行符
		n, err := conn.Read(buf)
		if err != nil {
			if err == io.EOF {
				fmt.Printf("客户端[%v]退出了!!!\n", addr)
				break
			} else {
				fmt.Printf("conn.Read() err:%v\n", err)
				return
			}
		}
		fmt.Printf("服务器读到的数据:%v", string(buf[:n]))
		// 处理：小写转大写，回写给客户端
		conn.Write(bytes.ToUpper(buf[:n]))
	}
}


// 服务端主函数（入口函数）
func main() {
	// 创建监听套接字，tcp 协议，监听端口号为 8080
	listener, err := net.Listen("tcp", "127.0.0.1:8080")
	if err != nil {
		fmt.Printf("net.Listen() err:%v\n", err)
		return
	}
	defer listener.Close()

	// 循环监听客户端连接请求
	for {
		fmt.Println("服务器等待客户端连接...")
		// 阻塞等待客户端连接
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("listener.Accept() err:%v\n", err)
			return
		}

		// 连接成功，就开一个协程处理：服务器和客户端的数据通信
		go HandleConnectReq(conn)
	}
}
