#include "os.h"

#define DELAY 1000

void user_task0(void)
{
	uart_puts("Task 0: Created!\n");
	while (1)
	{
		uart_puts("Task 0: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
void user_task1(void)
{
	uart_puts("Task 1: Created!\n");
	while (1)
	{
		uart_puts("Task 1: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
void user_task2(void)
{
	uart_puts("Task 2: Created!\n");
	while (1)
	{
		uart_puts("Task 2: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
void user_task3(void)
{
	uart_puts("Task 3: Created!\n");
	while (1)
	{
		uart_puts("Task 3: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
void user_task4(void)
{
	uart_puts("Task 4: Created!\n");
	while (1)
	{
		uart_puts("Task 4: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
void user_task5(void)
{
	uart_puts("Task 5: Created!\n");
	while (1)
	{
		uart_puts("Task 5: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void)
{
	task_create((void *)user_task0, NULL, 0, 2);
	// task_create((void *)user_task1, NULL, 0, 2);
	task_create((void *)user_task2, NULL, 1, 3);
	// task_create((void *)user_task3, NULL, 2, 1);
	// task_create((void *)user_task4, NULL, 244, 22);
	// task_create((void *)user_task5, NULL, 3, 1);
}
