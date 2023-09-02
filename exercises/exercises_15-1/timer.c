#include "os.h"

extern void schedule(void);

/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

#define MAX_TIMER 10
// static struct Node *timerList = NULL;

struct node timerList;
struct timer temp;
struct node temp_node;

/* load timer interval(in ticks) for next timer interrupt.*/
/* 加载下一个定时器中断的定时器间隔（以滴答为单位）。*/
void timer_load(int interval)
{
	/* each CPU has a separate source of timer interrupts. */
	/* 每个CPU都有一个单独的定时器中断源。 */
	int id = r_mhartid();

	*(uint64_t *)CLINT_MTIMECMP(id) = *(uint64_t *)CLINT_MTIME + interval;
}

void timer_init()
{
	timerList.next = NULL;
	timerList.pre = NULL;

	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	/*
	 * 复位时，mtime 清零，但 mtimecmp 寄存器不复位。
	 * 所以我们必须手动初始化 mtimecmp。
	 */
	timer_load(TIMER_INTERVAL);

	/* enable machine-mode timer interrupts. */
	/* 启用机器模式定时器中断。 */
	w_mie(r_mie() | MIE_MTIE);
}

struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout)
{
	/* TBD: params should be checked more, but now we just simplify this */
	/* TBD: 应该更多地检查参数，但现在我们只是简化它 */
	if (NULL == handler || 0 == timeout)
	{
		return NULL;
	}

	/* use lock to protect the shared timer_list between multiple tasks */
	/* 使用锁来保护多个任务之间共享的timer_list */
	spin_lock();

	// struct timer *temp = (struct timer *)malloc(sizeof(struct timer));
	// temp->func = handler;
	// temp->arg = arg;
	// temp->timeout_tick = _tick + timeout;
	// insert_node(&timerList, (void *)temp);]

	struct timer *time = &temp;
	temp.func = handler;
	temp.arg = arg;
	temp.timeout_tick = _tick + timeout;

	temp_node.timer = time;
	temp_node.next = NULL;

	if (timerList.next == NULL)
	{
		timerList.next = &temp_node;

		spin_unlock();
		return time;
	}

	struct node *current = &timerList;
	current = current->next;
	struct timer *temp_timer;
	while (current->next != NULL)
	{
		temp_timer = current->next->timer;
		if (temp_timer->timeout_tick > time->timeout_tick)
		{
			break;
		}
		current = current->next;
	}

	// temp_node.pre = current;
	temp_node.next = current->next;
	// if (current->next != NULL)
	// {
	// 	current->next->pre = &temp_node;
	// }
	current->next = &temp_node;
	printf("create timer current->%d   next->%d\n", current->timer->timeout_tick, current->next->timer->timeout_tick);

	spin_unlock();
	return time;
}

void timer_delete(struct timer *timer)
{
	spin_lock();

	// delete_node(&timerList, (void *)timer);

	spin_unlock();
}

/* this routine should be called in interrupt context (interrupt is disabled) */
/* 这个例程应该在中断上下文中调用（中断被禁用）*/
static inline void timer_check()
{

	// if (timerList == NULL || timerList->next == NULL)
	// 	return;
	// struct Node *current = timerList->next;
	// struct timer *temp = NULL;
	// while (current != NULL)
	// {
	// 	temp = (struct timer *)(current->data);
	// 	if (_tick >= temp->timeout_tick)
	// 	{
	// 		delete_node(&timerList, (void *)temp);
	// 		break;
	// 	}
	// 	current = current->next;
	// }

	struct node *current = &(timerList);
	// current = current->next;
	struct timer *temp;

	while (current->next != NULL)
	{
		temp = current->next->timer;
		if (_tick >= temp->timeout_tick)
		{
			printf("delete_node\n");
			// current->pre->next = current->next;
			temp->func(temp->arg);
			// temp.func(temp.arg);
			current->next = current->next->next;
			// if (current->next != NULL)
			// {
			// 	current->next->pre = current->pre;
			// }
			break;
		}

		current = current->next;
	}
}
void print()
{
	struct node *current = &timerList;
	current = current->next;
	while (current != NULL)
	{
		printf("print ndoe %d\n", current->timer->timeout_tick);
		current = current->next;
	}
}
void timer_handler()
{
	_tick++;
	printf("tick: %d\n", _tick);

	// print();

	timer_check();
	timer_load(TIMER_INTERVAL);

	schedule();
}
