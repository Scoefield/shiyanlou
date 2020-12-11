#include <iostream>

using namespace std;


// 判断大小端函数
void judge_big_little_end()
{
    cout << "本机器的字节序模式为：";
    int i = 1;                      // 定义 int 变量 i
    char c = (*(char*)&i);          // 将 i 的地址强转为 char* 类型，为了能够拿到 i 的地地址

    // 如果为 1，则为小端存储模式，否则为大端模式
    if (c)
        cout << "小端" <<endl;
    else
        cout << "大端" <<endl;
}


// 主函数
int main()
{
    // 调用判断大小端的函数
    judge_big_little_end();  
    return 0;
}
