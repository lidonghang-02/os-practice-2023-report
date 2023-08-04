#include <stdio.h>

int foo(int a, int b) {
    int c;
    
    asm volatile(
        "add %[c], %[a], %[b]\n" // 将a和b相加，结果存储到c
        : [c] "=r" (c)          // 输出操作数：将c约束为寄存器
        : [a] "r" (a), [b] "r" (b) // 输入操作数：将a和b约束为寄存器
    );
    
    return c;
}

int main() {
    printf("foo(1, 2) -> %d\n", foo(1, 2));
    return 0;
}
