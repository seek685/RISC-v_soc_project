#include <stdint.h>

// QEMU virt 平台的 CLINT 模块基地址
#define CLINT_BASE 0x2000000
// mtimecmp 寄存器地址 (Hart 0)
#define MTIMECMP_ADDR (CLINT_BASE + 0x4000)
// mtime 寄存器地址
#define MTIME_ADDR    (CLINT_BASE + 0xBFF8)

// 假设 QEMU 的时钟频率是 10MHz (10,000,000 ticks/sec)
// 我们设置 1 秒钟触发一次中断
#define TIMER_INTERVAL 10000000

volatile int timer_tick_count = 0;

// --- RV32 架构下安全写入 64 位 MMIO 寄存器的标准姿势 ---
void set_mtimecmp(uint64_t value) {
    volatile uint32_t *mtimecmp_lo = (uint32_t *)(MTIMECMP_ADDR);
    volatile uint32_t *mtimecmp_hi = (uint32_t *)(MTIMECMP_ADDR + 4);//区别地址与数据

    // 极其重要的防并发陷阱：
    // 因为我们是 32 位 CPU，写入 64 位需要分两步。
    // 如果先写低 32 位，此时高 32 位还是旧值，可能会瞬间满足 mtime >= mtimecmp，
    // 从而触发一个虚假的、错误的中断！
    // 解决办法：先将高 32 位写为全 1 (无穷大)，再写低 32 位，最后写入真实的高 32 位。
    *mtimecmp_hi = 0xFFFFFFFF;
    *mtimecmp_lo = (uint32_t)(value & 0xFFFFFFFF);
    *mtimecmp_hi = (uint32_t)(value >> 32);
}

// 读取 64 位 mtime 寄存器
uint64_t get_mtime() {
    volatile uint32_t *mtime_lo = (uint32_t *)(MTIME_ADDR);
    volatile uint32_t *mtime_hi = (uint32_t *)(MTIME_ADDR + 4);
    uint32_t lo, hi;
    do {
        hi = *mtime_hi;
        lo = *mtime_lo;
    } while (hi != *mtime_hi); // 防止在读取低位时，高位发生了进位
    return (((uint64_t)hi) << 32) | lo;
}

// 初始化定时器
void timer_init() {
    // 1. 读取当前时间，加上间隔，设置下一次中断的阈值
    uint64_t current_time = get_mtime();
    set_mtimecmp(current_time + TIMER_INTERVAL);

    // 2. 开启机器模式定时器中断使能 (mie 寄存器的 MTIE 位，第 7 位)
    // 使用内联汇编设置 CSR
    __asm__ volatile("csrs mie, %0" :: "r"(1 << 7));

    //用法：
    //__asm__ volatile("汇编指令" : 输出参数 : 输入参数);
    
    // 3. 开启全局中断使能 (mstatus 寄存器的 MIE 位，第 3 位)
    __asm__ volatile("csrs mstatus, %0" :: "r"(1 << 3));
}

// --- 升级版的 Trap 处理函数 ---
//压缩指令导致中断入口地址可能不是4的整数倍
__attribute__((aligned(4))) __attribute__((interrupt("machine"))) void trap_handler(void) {
    uint32_t mcause;
    // 读取 mcause 寄存器，判断 Trap 原因
    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));
    /**
    读取 CPU 内部 mcause 寄存器的值，并把它赋值给 C 语言里的 mcause 变量。
    这样，你的 C 代码就能通过 if (mcause == 0x80000007) 来判断是不是定时器中断了
    **/

    // mcause 的最高位如果是 1，代表是中断 (Interrupt)
    // 在 RV32 中，最高位为 1 即 0x80000000。
    // 机器定时器中断的异常码 (Exception Code) 是 7。
    // 所以 0x80000007 代表 Machine Timer Interrupt
    if (mcause == 0x80000007) {
        // 确实是定时器中断！
        timer_tick_count++; // 心跳变量 +1

        // 极其重要：必须更新 mtimecmp，否则中断会一直无限触发，导致死机！
        uint64_t current_time = get_mtime();
        set_mtimecmp(current_time + TIMER_INTERVAL);
    } else {
        // 其他异常（比如之前的 ecall，或者非法指令）
        while(1); 
    }
}

int main() {
    // 初始化并启动定时器
    timer_init();

    // 主循环：CPU 在这里做自己的事情，定时器会在后台默默打断它
    while(1) {
        // 可以在这里打断点，或者什么都不做
        __asm__ volatile("nop"); 
    }
    return 0;
}