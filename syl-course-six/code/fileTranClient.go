/*
	file: fileTranClient.go
	runCmd: go run fileTranClient.go fileName.txt
*/
package main

import (
	"fmt"
	"io"
	"net"
	"os"
)

// 发送文件内容
func sendFileContent(conn net.Conn, fileName string) {
	// 打开文件，默认是以只读的方式打开
	file, err := os.Open(fileName)
	if err != nil {
		fmt.Printf("os.Open()函数执行出错，错误信息为:%v\n", err)
		return
	}
	// 延迟关闭文件句柄
	defer file.Close()

	buf := make([]byte, 4096)
	for {
		// 读取文件内容写到缓冲区
		n, err := file.Read(buf)
		if err != nil {
			if err == io.EOF {
				fmt.Println("发送文件结束")
			} else {
				fmt.Printf("file.Read()方法执行错误,错误信息为:%v\n", err)
			}
			return
		}
		// 将文件内容发送给服务端(接收端)
		_, err = conn.Write(buf[:n])
	}
}

// 客户端（发送端）主函数
func main() {

	// 使用go语言的标准库os.Args获取命令行参数，返回的是字符串列表
	args := os.Args

	// 参数数量判断
	if len(args) != 2 {
		fmt.Printf("格式为: go run fileTranClient.go fileName\n")
		return
	}

	// 获取文件名参数
	filePath := args[1]

	// 获取文件属性
	fileAttr, err := os.Stat(filePath)
	if err != nil {
		fmt.Printf("os.Stat()函数执行出错，错误信息为:%v\n", err)
		return
	}

	// 发起连接请求，tcp协议，端口号8080，注意这里的端口号要与服务端（接收端）的对应上
	conn, err := net.Dial("tcp", "127.0.0.1:8080")
	if err != nil {
		fmt.Printf("net.Dial()函数执行出错，错误信息为:%v\n", err)
		return
	}
	// 延迟关闭连接套接字
	defer conn.Close()

	// 发送文件名给服务端（接收端）
	_, err = conn.Write([]byte(fileAttr.Name()))

	// 读取服务端（接收端）回发的数据
	buf := make([]byte, 4096)
	n, err := conn.Read(buf)
	if err != nil {
		fmt.Printf("conn.Read()方法执行出错，错误信息为:%v\n", err)
		return
	}

	// 如果收到服务端（接收端）的应答为"ok"，则写文件内容给服务端
	if string(buf[:n]) == "ok" {
		sendFileContent(conn, filePath)
	}
}
