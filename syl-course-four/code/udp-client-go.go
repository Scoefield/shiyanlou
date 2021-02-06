/*
    file: udp-client-go.go
*/
package main

import (
    "fmt"
    "net"
)

func main() {
	// 1、创建客户端连接，返回创建的连接结构 conn，注意这里的端口要与服务端对应（8000）
    conn, err := net.Dial("udp", "127.0.0.1:8000")
    if err != nil {
        fmt.Printf("net.Dial()函数执行出错,错误信息为:%v\n", err)
        return
	}
	// 同样也是利用 defer 延迟关闭 udpSocket，即函数执行完后关闭
    defer conn.Close()

	// 2、客户端写数据到服务端
    conn.Write([]byte("Hello, this is udp-client send message."))

	// 3、发送完数据后创建一个缓冲区 buf 来存储从服务端读回来的数据
    buf := make([]byte, 4096)
    n, err := conn.Read(buf)
    if err != nil {
        fmt.Printf("Conn.Read()方法执行出错，错误信息为:%v\n", err)
        return
	}
	// 打印读取到的数据
    fmt.Printf("从服务器读取到的数据为：%s\n", buf[:n])
}
