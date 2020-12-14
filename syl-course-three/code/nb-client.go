package main

import (
	"fmt"
	"net"
)

func main() {
	conn, err := net.Dial("tcp", "127.0.0.1:8888")
	if err != nil {
		fmt.Println("dial failed, err", err)
		return
	}
	defer conn.Close()
	// 分 20 次发送 msg 
	for i := 0; i < 20; i++ {
		msg := `Hello, World. How are you?`
		conn.Write([]byte(msg))
	}
}
