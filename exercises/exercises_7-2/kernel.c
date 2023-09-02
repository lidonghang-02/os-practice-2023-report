/*
 * @Author: lidonghang-02 2426971102@qq.com
 * @Date: 2023-08-08 12:23:17
 * @LastEditors: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-08-30 23:12:14
 * @FilePath: /riscv/exercises/exercises_7-2/kernel.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

extern void uart_init(void);
extern void uart_puts(char *s);
extern void uart_putc(char c);
extern int uart_getc(void);

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, RVOS!\n");

	char buffer[1024];

	while (1)
	{
		uart_isr();
	}

	// while (1)
	// {
	// }; // stop here!
}
