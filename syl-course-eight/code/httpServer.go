/*
	file：httpServer.go
	runCmd：go run httpServer.go
 */

 package main

 import ()
	 "fmt"
	 "net/http"
 )
 
 // 处理函数，w 写数据给客户端，r 获取客户端发送过来的数据
 func HelloHandler(w http.ResponseWriter, r *http.Request)  {
	 // 返回打印信息
	 fmt.Fprintln(w, "Hello golang，this is handler response.")
 
	 // 请求方法、URL、请求头信息、请求体
	 respMsg := fmt.Sprintf(" method: %v\n url: %v\n header: %v\n body: %v\n", r.Method, r.URL, r.Header, r.Body)
 
	 // 返回 respMsg 数据给客户端
	 _, err := w.Write([]byte(respMsg))
	 // 错误处理
	 if err != nil {
		 fmt.Println(err)
	 }
 }
 
 
 // 主函数
 func main() {
	 // 注册路由及处理函数，用户请求连接后，会调用该 HelloHandler 函数处理
	 http.HandleFunc("/hello", HelloHandler)
 
	 // 监听并绑定地址和端口，这里用的是本地地址
	 err := http.ListenAndServe("127.0.0.1:8080", nil)
	 // 错误处理
	 if err != nil {
		 fmt.Println(err)
	 }
 }
 