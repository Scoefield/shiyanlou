/*
	file: fileTranServer.go
	runCmd: go run fileTranServer.go
*/
package main

import (
	"fmt"
	"io"
	"net"
	"os"
)

const (
	MAXBUF = 4096 			// buf缓存区大小
	TCP = "tcp"				// tcp 协议
	ADDR = "127.0.0.1:8080"	// 监听地址
)

// 处理接收文件内容
func recvFileContent(conn net.Conn, fileName string) {
	// 根据文件名创建新文件
	fmt.Println("根据新的文件名来创建文件......")
	file, err := os.Create(fileName)
	if err != nil {
		fmt.Printf("os.Create()函数执行错误，错误信息为:%v\n", err)
		return
	}
	fmt.Println("创建文件成功.")
	// 延迟关闭文件句柄
	defer file.Close()

	// 循环接收客户端发过来的数据，然后写入上面创建的文件
	fmt.Println("开始接收客户端发送过来的文件内容数据，并写入文件......")
	for {
		buf := make([]byte, MAXBUF)
		n, err := conn.Read(buf)
		// 读到的数据写入文件
		file.Write(buf[:n])
		if err != nil {
			if err == io.EOF {	// io.EOF 表示读取完文件内容
				fmt.Printf("接收文件内容完成，请查看文件%s里的内容是否正确.\n", fileName)
			} else {
				fmt.Printf("conn.Read()方法执行出错，错误信息为:%v\n", err)
			}
			return
		}
	}
}

// 服务端（接收端）主函数
func main() {
	// 创建监听的套接字 socket，使用 tcp 协议，端口号8080
	listener, err := net.Listen(TCP, ADDR)
	if err != nil {
		fmt.Printf("net.Listen()方法执行错误，错误信息为:%v\n", err)
		return
	}
	// 延迟关闭监听套接字 listenner
	defer listener.Close()

	// 阻塞监听，等待客户端连接的到来
	fmt.Println("等待客户端连接的到来......")
	conn, err := listener.Accept()
	if err != nil {
		fmt.Printf("listener.Accept()方法执行错误，错误信息为:%v\n", err)
		return
	}
	fmt.Println("连接客户端成功.")
	// 延迟关闭连接套接字 conn
	defer conn.Close()

	// 定义一个缓冲区变量 buf，用于存储接收到的数据
	// 此处用于保存客户端传过来的文件名
	buf := make([]byte, 4096)
	fmt.Println("开始接收客户端端发送过来的文件名数据......")
	n, err := conn.Read(buf)
	if err != nil {
		fmt.Printf("conn.Read()方法执行错误，错误信息为:%v\n", err)
		return
	}
	// 文件名前缀加"server"，重新定义为一个新文件
	// 在本地测试时，为了区分是服务端还是客户端的文件
	fileName := "server" + string(buf[:n])
	fmt.Printf("接收文件名成功，接收到的文件名为：%s，加前缀后的新文件名为：%s\n", string(buf[:n]), fileName)

	// 接收文件名信息成功后，回写ok给客户端（发送端）
	fmt.Println("开始回写OK信息给客户端......")
	n, err = conn.Write([]byte("ok"))
	if err != nil {
		fmt.Printf("conn.Write()方法执行错误，错误信息为:%v\n", err)
		return
	}
	fmt.Println("回写OK信息成功.")

	// 单独封装一个函数来处理获取文件的内容
	recvFileContent(conn, fileName)
}
