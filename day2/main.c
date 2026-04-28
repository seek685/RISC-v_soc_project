// main.c
int my_math_add(int a, int b) {
    return a + b;
}

int main(void) {
    int x = 10;
    int y = 20;
    int result = 0;

    // 调用一个简单的加法函数
    result = my_math_add(x, y);

    // 死循环，防止 main 函数跑飞退出
    while(1) {
        result++;
    }

    return 0;
}