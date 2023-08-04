<p>
    <center><font size=8 face="楷体">操作系统实践课程结课报告</font></center><br>
	<center><font size=5 face="楷体">目标：上下文切换和协作式多任务</font></center>
	<center><font face="楷体">作者：李东航（学号：20212241283）</font></center>
	<center><font face="楷体">日期：2023年8月</font></center>
</p>

# 基础概念
1. 任务（task）
- 任务是一条指令的执行流，是函数的执行过程。
![](pic/v0.2/Pasted%20image%2020230803162023.png)
2. 多任务（Multiatask）
- 多任务:多条执行流，可以是多核多任务，也可以是单核多任务。
- 多核多任务:不同的核执行不同的执行流.
- 单核多任务:一个核轮流执行不同的执行流。
![](pic/v0.2/Pasted%20image%2020230804093730.png)
3. 任务上下文切换（Context）
- 上下文切换是指当操作系统从一个进程或线程切换到另一个进程或线程时，保存当前进程或线程的状态，包括寄存器值、程序计数器值、堆栈指针等，然后恢复下一个进程或线程的状态。
![](pic/v0.2/Pasted%20image%2020230804093902.png)
# 协作式多任务的设计
> 协作式多任务：协作式环境下，下一个任务被调度的前提是当前任务主动放弃处理器

1. 设计思路
-  `switch_to`中首先保存上一个任务的上下文，然后切换任务，并加载新任务的上下文，最后调用`return`实现新任务
-  `ra（x1）`用于存储返回地址
 - `mscratch`相当于指针，用于切换任务
 ![](pic/v0.2/Pasted%20image%2020230804100005.png)
2. 执行过程
假设Task A先执行，首先会初始化，Context A中的`ra(x1)`会保存Task A第一条指令的地址，CPU的`mscratch`指向这个地址。
当运行到`call switch_to`时，CPU中的`ra(x1)`就会保存下一条指令的地址，然后跳转到`switch_to`函数，会保存Task A的上下文，即把CPU中的`ra`的值给Context A中的`ra`。然后加载Task B中的`ra`值，即CPU中的`ra = j`，同时`mscratch`指向Context B的地址，最后的`ret`指令会跳转到CPU中`ra`的地址，开始Task B，实现任务切换
# 代码实现
1. 关键函数（switch_to）
```assembly
# void switch_to(struct context *next);
# a0: 指向下一个任务的上下文的指针
.globl switch_to
.align 4
switch_to:
	csrrw	t6, mscratch, t6	# 交换 t6 和 mscratch
	beqz	t6, 1f			    # 注意: 第一次调用 switch_to() 时，mscratch 被初始化为零（在 sched_init() 中），这会使 t6 为零，这是我们必须处理的特殊情况。
	reg_save t6			        # 保存前一个任务的上下文

    # 保存实际的 t6 寄存器，我们将其交换到了 mscratch。
	mv	t5, t6	            	# t5 指向当前任务的上下文
	csrr	t6, mscratch	    # 从 mscratch 中读回 t6
	sw	t6, 120(t5)	            # 以 t5 为基地址，保存 t6

1:
	# 切换 mscratch，使其指向下一个任务的上下文
	csrw	mscratch, a0

	# 恢复所有 GP 寄存器
	# 使用 t6 指向新任务的上下文
	mv	t6, a0
	reg_restore t6

	# 进行实际的上下文切换。
	ret
```
2. 创建和初始化任务
定义任务的上下文，context结构体中存储一个任务的上下文信息
```c
struct context {
/* ignore x0 */
reg_t ra;
reg_t sp;
reg_t gp;
reg_t tp;
reg_t t0;
reg_t t1;
reg_t t2;
reg_t s0;
reg_t s1;
reg_t a0;
reg_t a1;
reg_t a2;
reg_t a3;
reg_t a4;
reg_t a5;
reg_t a6;
reg_t a7;
reg_t s2;
reg_t s3;
reg_t s4;
reg_t s5;
reg_t s6;
reg_t s7;
reg_t s8;
reg_t s9;
reg_t s10;
reg_t s11;
reg_t t3;
reg_t t4;
reg_t t5;
reg_t t6;
};
```
定义任务的栈和上下文,此时最多10个任务
```c
#define MAX_TASKS 10
#define STACK_SIZE 1024
uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE];
struct context ctx_tasks[MAX_TASKS];
```
task0和task1的执行内容
```c
void user_task0(void)
{
	uart_puts("Task 0: Created!\n");
	while (1) {
		uart_puts("Task 0: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
void user_task1(void)
{
	uart_puts("Task 1: Created!\n");
	while (1) {
		uart_puts("Task 1: Running...\n");
		task_delay(DELAY);
		task_yield();
	}
}
```
任务的创建函数,`ra`是上面任务内容的地址
```c
int task_create(void (*start_routin)(void))
{
	if (_top < MAX_TASKS) {
		ctx_tasks[_top].sp = (reg_t) &task_stack[_top][STACK_SIZE];
		ctx_tasks[_top].ra = (reg_t) start_routin;
		_top++;
		return 0;
	} else {
		return -1;
	}
}
```
任务调度的实现
```c
static int _top = 0;
static int _current = -1;
void schedule()
{
	if (_top <= 0) {
		panic("Num of task should be greater than zero!");
		return;
	}
	_current = (_current + 1) % _top;
	struct context *next = &(ctx_tasks[_current]);
	switch_to(next);
}
```
# 代码运行
![](pic/v0.2/Pasted%20image%2020230804103108.png)


