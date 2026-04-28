.global _start   # 告诉编译器程序的入口在哪里
    .text            # 声明下面是代码段

_start:
    # 1. 模拟设置一个栈顶指针 (QEMU virt 平台的 RAM 从 0x80000000 开始)
    li sp, 0x80010000  
    
    # 2. 往寄存器里放点测试数据
    li ra, 0x11223344
    li a0, 0x8899aabb
    li a1, 0x66666666
    li a2, 0x77777777
    # 3. 栈指针下移 8 个字节，准备开辟空间
    addi sp, sp, -16

    # 4. 把寄存器的值存入内存 (压栈)
    sw ra, 12(sp)
    sw a0, 8(sp)
    sw a1, 4(sp)
    sw a2, 0(sp)

    # 5. 模拟搞破坏，清空寄存器
    li ra, 0
    li a0, 0
    li a1, 6
    li a2,7

    # 6. 从内存恢复数据 (出栈)
    lw ra, 12(sp)
    lw a0, 8(sp)
    lw a1, 4(sp)
    lw a2, 0(sp)
    # 7. 恢复栈指针
    addi sp, sp, 16

end_loop:
    j end_loop       # 死循环，防止程序跑飞