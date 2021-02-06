/*
    file: udp-server-go.go
*/
package main

import (
    "fmt"
    "net"
)

func main() {
    // 1、创建一个 *UDPAddr，即一个 udp 地址结构，传入 udp 协议并指定服务器的 IP 和 port，这里的端口为8000
    udpAddr, err := net.ResolveUDPAddr("udp", "127.0.0.1:8000")
    if err != nil {
        fmt.Printf("net.ResolveUDPAddr() 函数执行出错, 错误信息为: %v\n", err)
        return
    }
    fmt.Println("UDP 服务器地址结构创建成功!")

    // 2、创建服务端（用户通信）的 socket，将第一步创建的 *UDPAddr类型的参数传入
    udpSocket, err := net.ListenUDP("udp", udpAddr)
    if err != nil {
        fmt.Printf("net.ListenUDP()函数执行出错,错误为:%v\n", err)
        return
	}
	// 利用 defer 延迟关闭 udpSocket，即函数执行完后关闭
    defer udpSocket.Close()
    fmt.Println("UDP 服务器端通信的 socket 创建成功，等待客户端连接...")

    // 3、创建 buf 缓冲区存放读取客户端发送的数据
    buf := make([]byte, 4096)
    // ReadFromUDP()方法会返回三个值，分别是读取到的字节数、客户端的地址以及error；ReadFromUDP()方法会阻塞，直到客户端连接进来
	n, clientUDPAddr, err := udpSocket.ReadFromUDP(buf)
    if err != nil {
        fmt.Printf("ReadFromUDP()方法执行出错，错误信息为:%v\n", err)
        return
	}
	fmt.Println("连接成功，客户端地址为：", clientUDPAddr)

    // 4、模拟处理读取到客户端的数据
    fmt.Printf("服务器读到[%v]发送过来的数据为: %s", clientUDPAddr, buf[:n])

    // 5、处理完数据后，回写数据给客户端，这里随便写了一个字符串过去，使用 WriteToUDP() 方法
    _, err = udpSocket.WriteToUDP([]byte("Data processing completed!"), clientUDPAddr)
    if err != nil {
        fmt.Printf("WriteToUDP()方法执行出错，错误信息为:%v\n", err)
        return
    }
}
