package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

// 客户端主函数
func main() {
	// 创建与服务器端连接
	conn, err := net.Dial("tcp", "127.0.0.1:6666")
	if err != nil {
		fmt.Println("err :", err)
		return
	}
	defer conn.Close() 							 // 延迟关闭连接（函数执行完就关闭连接）

	inputReader := bufio.NewReader(os.Stdin)	 // 创建带缓冲的系统标准输入的句柄
	for {
		input, _ := inputReader.ReadString('\n') // 读取用户输入的内容
		inputInfo := strings.Trim(input, "\r\n") // 去掉换行符
		if strings.ToUpper(inputInfo) == "Q" {   // 如果输入q就退出
			return
		}
		_, err = conn.Write([]byte(inputInfo))   // 向服务端发送数据
		if err != nil {
			return
		}

		buf := [512]byte{}
		n, err := conn.Read(buf[:])				// 接收数据
		if err != nil {
			fmt.Println("recv failed, err:", err)
			return
		}
		fmt.Println(string(buf[:n]))			// 打印接收到的数据
	}
}
