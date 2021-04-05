/*
	file: client/main.go
*/
package main

import (
	"fmt"
	"os"
	"encoding/json"
)

// 发送消息的结构体
type Message struct {
	UserId int	`json:"userId"`		// 用户ID
	UserPwd string `json:"userPwd"`	// 用户密码
}

var UserId	int
var UserPwd string

// 登录逻辑
func login(UserId int, UserPwd string) error {
	// fmt.Printf("UserId=%d, UserPwd=%s\n", UserId, UserPwd)
	// return nil

	// 1.连接到服务器
	conn, err := net.Dial("tcp", "localhost:8080")
	if err != nil {
		fmt.Println("net.Dial err:", err)
		return
	}
	// 延时关闭 conn
	defer conn.Close()

	// 声明 message 用于存放消息
	var message Message
	message.UserId = UserId
	message.UserPwd = UserPwd

	// 序列号 message 结构体
	data, err := json.Marshal(message)
	if err != nil {
		fmt.Println("序列化错误，err:", err)
		return
	}

	// 打印客户端发送的数据内容
	fmt.Println("客户端发送的内容为：", string(data))

	// 发送数据 data 给服务端
	n, err := conn.Write(data)
	if err != nil {
		fmt.Println("conn.Write err:", err)
		return
	}

	return
}

func main() {
	var key int
	var loop = true
	for loop {
		fmt.Println("*********** 欢迎进入通讯系统 **********")
		fmt.Println("\t\t 1 登录通讯室")
		fmt.Println("\t\t 2 注册用户")
		fmt.Println("\t\t 3 退出系统")
		fmt.Println("\t\t 请选择（1-3）")

		fmt.Scanf("%d\n", &key)

		switch key {
		case 1:
			fmt.Println("登录通讯室")
			loop = false
		case 2:
			fmt.Println("注册用户")
			loop = false
		case 3:
			fmt.Println("退出系统")
			// loop = false
			os.Exit(0)
		default:
			fmt.Println("您的输入有误，请重新输入")
		}
	}

	// 用户选择后的处理逻辑
	if key == 1 {
		fmt.Println("请输入用户id:")
		fmt.Scanf("%d\n", &UserId)
		fmt.Println("请输入用户密码:")
		fmt.Scanf("%s\n", &UserPwd)

		// 登录逻辑
		err := login(UserId, UserPwd)
		if err != nil {
			fmt.Println("登录失败！")
		}else {
			fmt.Println("登录成功")
		}
	}else if key == 2 {
		fmt.Println("这里执行注册的逻辑...")
	}
}


