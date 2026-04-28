/*day3:link.ld*/

/*1.告诉编译器，芯片入口函数是_start*/
ENTRY(_start)

/*2.定义物理内存分布（memory map）*/
/*假设有一块64KB的ram，从0x80000000开始*/
MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 64K
}

/*3.定义各个段怎么分布*/
SECTIONS
{
    /*第一部分 代码段*/
    .text :
    {
        *(.text)   /*把所有的.text段都放这里*/
    } > RAM
    .data :
    {
        *(.data) 
    } > RAM
    .bss :
    {       /*在链接脚本里，小数点 . 代表“当前的物理内存地址*/
         _sbss = .;      /* 记号1：记录当前地址为 BSS 的起始地址 */
        *(.bss)           /* 存放所有的未初始化全局变量 */
         *(.bss.*)       /* 匹配可能带后缀的 BSS 段 */
        *(.sbss)        /* RISC-V 特有的 Small BSS 段（你的变量在这里！）*/
        *(.sbss.*)      
        *(COMMON)       /* C 语言标准的公共符号段 */
         _ebss = .;      /* 记号2：记录当前地址为 BSS 的结束地址 */
    } > RAM
}
