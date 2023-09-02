#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context *next);

#define MAX_TASKS 10
#define STACK_SIZE 1024
/*
 * In the standard RISC-V calling convention, the stack pointer sp
 * is always 16-byte aligned.
 */
uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE];
struct context ctx_tasks[MAX_TASKS];

/*
 * _top is used to mark the max available position of ctx_tasks
 * _current is used to point to the context of current task
 */
static int _top = 0;
static int _current = -1;

static void w_mscratch(reg_t x)
{
	asm volatile("csrw mscratch, %0"
		     :
		     : "r"(x));
}

void sched_init()
{
	w_mscratch(0);
}

/*
 * implment a simple cycle FIFO schedular
 */
void schedule()
{
	if (_top <= 0)
	{
		panic("Num of task should be greater than zero!");
		return;
	}
	for (int i = 0; i < _top; i++)
	{
		if (ctx_tasks[i].running == 0)
			continue;

		if (ctx_tasks[i].flag > 0)
		{
			ctx_tasks[i].flag--;
		}
		else
		{
			ctx_tasks[i].flag = ctx_tasks[i].priority;
			_current = i;
			switch_to(&(ctx_tasks[i]));
		}
	}
}

/*
 * DESCRIPTION
 * 	Create a task.
 * 	- start_routin: task routine entry
 * RETURN VALUE
 * 	0: success
 * 	-1: if error occured
 */
int task_create(void (*task)(void *param), void *param, uint8_t priority)
{
	if (_top < MAX_TASKS)
	{
		ctx_tasks[_top].sp = (reg_t)&task_stack[_top][STACK_SIZE];
		ctx_tasks[_top].ra = (reg_t)task;
		ctx_tasks[_top].param = param;
		ctx_tasks[_top].priority = priority;
		ctx_tasks[_top].flag = 0;
		ctx_tasks[_top].running = 1; // 任务运行状态

		_top++;
		return 0;
	}
	else
	{
		return -1;
	}
}

void task_exit(void)
{
	if (_top > 0)
	{
		_top--;
		printf("exit task id:%d\n", _current);
		ctx_tasks[_current].running = 0;
		schedule();
	}
}

/*
 * DESCRIPTION
 * 	task_yield()  causes the calling task to relinquish the CPU and a new
 * 	task gets to run.
 */
void task_yield()
{
	schedule();
}

/*
 * a very rough implementaion, just to consume the cpu
 */
void task_delay(volatile int count)
{
	count *= 50000;
	while (count--)
		;
}
