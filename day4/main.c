// main.c
int global_bss_var;

// 这是一个极其简陋的 Trap 处理函数
// __attribute__((interrupt)) 告诉编译器这是一个中断处理函数
// 编译器会自动帮我们在函数开头和结尾加上保存/恢复寄存器的汇编代码，并以 mret 结尾
__attribute__((interrupt("machine"))) void trap_handler(void) {
    // 发生异常时，程序会跳到这里
    // 我们在这里死循环，方便 GDB 抓住它
    while(1) {
        global_bss_var++; 
    }
}

int main() {
    // 故意触发一个环境调用异常 (Environment Call)
    // ecall 指令通常用于操作系统系统调用，在裸机下会触发一个同步异常
    __asm__ volatile("ecall");

    // 如果 Trap 配置失败，程序会跑飞，不会执行到这里
    while(1) {
        global_bss_var--;
    }
    return 0;
}