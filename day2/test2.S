.global _start
    .text

_start:
    # 1. 初始化栈指针 (给 C 语言的局部变量腾出空间)
    li sp, 0x80010000  

    # 2. 跳转到 C 语言的 main 函数！
    # call 是一个伪指令，它会自动把下一条指令的地址存入 ra 寄存器，然后跳到 main
    call main

    # 3. 如果 main 函数意外退出了，就死循环停在这里
end_loop:
    j end_loop