# day3/start.S
    .global _start
    .text

_start:
    # 1. 初始化栈指针
    li sp, 0x80010000  

    # ==========================================
    # 2. 清零 BSS 段 (内存清洁工开始工作)
    # ==========================================
    # la 指令 (Load Address)：把链接脚本里的记号地址，加载到寄存器里
    la t0, _sbss        # t0 = BSS 段的起始地址
    la t1, _ebss        # t1 = BSS 段的结束地址

clear_bss_loop:
    # bge 指令 (Branch if Greater or Equal)：如果 t0 >= t1，说明清零结束，跳出循环
    bge t0, t1, clear_bss_done  

    # sw 指令：把 zero 寄存器(永远是0)的值，存入 t0 指向的内存地址
    sw zero, 0(t0)      

    # addi 指令：t0 地址往后挪 4 个字节 (准备清零下一个 int)
    addi t0, t0, 4      

    # j 指令：跳回循环开头，继续判断
    j clear_bss_loop    

clear_bss_done:

    # --- 配置 Trap 向量基地址 ---
    # la 指令将 trap_handler 函数的地址加载到 t0 寄存器
    la t0, trap_handler
    
    # csrw (Control and Status Register Write) 指令
    # 将 t0 的值写入 mtvec 寄存器
    csrw mtvec, t0

    # --- 准备进入 C 环境 ---
    call main

    j .