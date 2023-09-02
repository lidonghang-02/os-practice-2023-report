#include "os.h"

/* interval ~= 1s */
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

static uint32_t _tick = 0;

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
	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	/*
	 * 在复位时，mtime被清零，但mtimecmp寄存器不会被复位。
	 * 因此，我们需要手动出事花mtimecmp
	 */
	timer_load(TIMER_INTERVAL);

	/* enable machine-mode timer interrupts. */
	// 启动机器模式的定时器中断
	w_mie(r_mie() | MIE_MTIE);

	/* enable machine-mode global interrupts. */
	// 启动机器模式的全局中断
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void timer_handler()
{
	_tick++;
	printf("tick: %d\n", _tick);

	uint32_t seconds = _tick % 60;
	uint32_t minutes = (_tick / 60) % 60;
	uint32_t hours = (_tick / 3600) % 24;
	// printf("\b\b\b\b\b\b\b\b\b");
	// printf("\033[2K");
	// printf("\033[1G");
	// printf("%02d:%02d:%02d", hours, minutes, seconds);
	// printf("\033[1B");

	timer_load(TIMER_INTERVAL);
}

uint32_t get_tick(void)
{
	return _tick;
}