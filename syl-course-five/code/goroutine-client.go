/*
	file: goroutine-client.go
	run-cmd: go run goroutine-client.go
*/

package main

import (
	"fmt"
	"io"
	"net"
	"os"
)

// 专门处理获取标准输入（键盘输入os.Stdin），并将输入数据发送到服务端
func HandleData(conn net.Conn) {
	str := make([]byte, 1024)
	for {
		// 获取标准输入
		n, err := os.Stdin.Read(str)
		if err != nil {
			fmt.Printf("os.Stdin.Read() err:%v\n", err)
			continue
		}
		// 发送输入的数据给服务端
		conn.Write(str[:n])
	}
}

// 客户端主函数（入口函数）
func main() {
	// 建立连接请求，tcp 协议，连接端口号 8080，与服务端的对应上
	conn, err := net.Dial("tcp", "127.0.0.1:8080")
	if err != nil {
		fmt.Printf("net.Dial() err:%v\n", err)
		return
	}
	defer conn.Close()

	// 开启一个协程（HandleData为处理函数），专门处理获取标准输入（键盘输入os.Stdin），并将输入数据发送到服务端
	go HandleData(conn)

	// 循环读取服务端发过来的数据，写到 buf
	for {
		buf := make([]byte, 1024)
		n, err := conn.Read(buf)
		if err != nil {
			if err == io.EOF {
				fmt.Println("服务端退出了!!!")
				return
			} else {
				fmt.Printf("conn.Read() err:%v\n", err)
				continue
			}
		}
		fmt.Printf("客户端收到到服务器发送回来的数据:%s", buf[:n])
	}
}
