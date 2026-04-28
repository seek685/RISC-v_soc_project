// day3/main.c

// 这是一个有初始值的全局变量 -> 编译器会把它放进 .data 段
int global_data_var = 8888; 

// 这是一个没有初始值的全局变量 -> 编译器会把它放进 .bss 段
int global_bss_var;         

int my_math_add(int a, int b) {
    return a + b;
}

int main(void) {
    int x = 10;
    int y = 20;
    int result = 0;

    // 我们把全局变量也加进来算一下
    result = my_math_add(x, y) + global_data_var;

    while(1) {
        result++;
    }

    return 0;
}