/*
	file: server/main.go
*/
package main

import (
	"net"
)

type Message struct {
	UserId int	`json:"userId"`		// 用户ID
	UserPwd string `json:"userPwd"`	// 用户密码
}

type LoginResMsg struct {
	Code int `json:"code"`
	ErrMsg string `json:"errMsg"`
}

func chatProcess(conn net.Conn) {
	// 延时关闭 conn
	defer conn.Close()

	// 循环处理客户端的信息
	for {
		buf := make([]byte, 4096)
		fmt.Println("获取客户端发送过来的信息...")
		n, err := conn.Read(buf[:1024])
		if err != nil {
			if err == io.EOF {
				fmt.Println("client exit, then server exit...")
				return
			}else {
				fmt.Println("conn.Read err:", err)
				return
			}
		}
		fmt.Println("读取到的数据：", buf[:n])
		
		// 反序列化数据
		var msg Message
		err = json.Unmarshal(buf[:1024], &msg)
		if err != nil {
			fmt.Println("json.Unmarshal err:", err)
			return
		}

		// 判断用户信息是否正确
		var loginResMsg LoginResMsg
		if msg.userId == 1001 && msg.userPwd == "dys123456" {
			loginResMsg.Code = 200
		}else {
			loginResMsg.Code = 500
			loginResMsg.ErrMsg = "用户不存在，请检查！"
		}
		data, err := json.Marshal(loginResMsg)
		if err != nil {
			fmt.Println("json.Marshal err:", err)
			return
		}
	}
}

func main() {
	listen, err := net.Listen("tcp", "0.0.0.0:8080")
	if err != nil {
		fmt.Println("net.Listen err:", err)
		return
	}

	// 监听客户端连接的到来
	for {
		fmt.Println("等待客户端连接的到来...")
		conn, err := listen.Accept()
		if err != nil {
			fmt.Println("listen.Accept err:", err)
		}
		// 连接成功，开启一个协程与客户端通讯
		go dataProcess()
	}
}