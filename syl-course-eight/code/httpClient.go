/*
	file：httpClient.go
	runCmd：go run httpClient.go
*/

package main

// 导入使用到的包
import (
	"fmt"
	"io/ioutil"
	"net/http"
)

// 客户端的主函数
func main() {
	// 服务端地址（http://127.0.0.1）、端口号（8080）、请求路径（/hello）
	url := "http://127.0.0.1:8080/hello"
	// 发起 get 请求
	resp, err := http.Get(url)
	// 出错打印报错信息
	if err != nil {
		fmt.Println("http.Get err, errMsg=", err)
		return
	}

	// 延迟关闭 body io
	defer resp.Body.Close()
	// 读取响应数据
	body, err := ioutil.ReadAll(resp.Body)
	// 读取报错信息处理
	if err != nil {
		fmt.Printf("ioutil.ReadAll()函数执行出错，错误为: %v\n", err)
		return
	}

	// 最后打印 接收到的响应数据
	fmt.Println("Get response data:")
	fmt.Println(string(body))
}
