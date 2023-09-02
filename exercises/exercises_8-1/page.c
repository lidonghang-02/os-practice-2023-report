#include "os.h"

/*
 * Following global vars are defined in mem.S
 */
// 在mem.S文件中定义的全局变量
extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t RODATA_START;
extern uint32_t RODATA_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

/*
 * _alloc_start points to the actual start address of heap pool
 * _alloc_end points to the actual end address of heap pool
 * _num_pages holds the actual max number of pages we can allocate.
 */
// _alloc_start 指向堆池的实际起始地址
// _alloc_end 指向堆池的实际结束地址
// _num_pages 存储我们能够分配的最大页面数
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define PAGE_TAKEN (uint8_t)(1 << 0)
#define PAGE_LAST (uint8_t)(1 << 1)

/*
 * Page Descriptor
 * flags:
 * - bit 0: flag if this page is taken(allocated)
 * - bit 1: flag if this page is the last page of the memory block allocated
 */
// 页面描述符
// flags:
// - bit 0: 如果该页面已经被分配，则为1
// - bit 1: 如果该页面是分配的内存块的最后一页，则为1
struct Page
{
	uint8_t flags;
	uint8_t malloc_flag;
	uint32_t _start;
	uint32_t _end;
};

// 清除页面描述符
static inline void _clear(struct Page *page)
{
	page->flags = 0;
}

// 判断页面是否空闲
static inline int _is_free(struct Page *page)
{
	if (page->flags & PAGE_TAKEN)
	{
		// flag的bit 0为1，页面已经被分配
		return 0;
	}
	else
	{
		return 1;
	}
}

// 设置页面标志
static inline void _set_flag(struct Page *page, uint8_t flags)
{
	// 将flag中设置为1的位设置到page->flag中
	// 同时保留page->flag中其他位的值
	page->flags |= flags;
}

// 判断页面是否为内存块的最后一页
static inline int _is_last(struct Page *page)
{
	if (page->flags & PAGE_LAST)
	{
		// flag的bit 1为1,是最后一页
		return 1;
	}
	else
	{
		return 0;
	}
}

// 将地址对齐到页面(4K)边界
static inline uint32_t _align_page(uint32_t address)
{
	uint32_t order = (1 << PAGE_ORDER) - 1;
	return (address + order) & (~order);
}

void page_init()
{
	/*
	 * We reserved 8 Page (8 x 4096) to hold the Page structures.
	 * It should be enough to manage at most 128 MB (8 x 4096 x 4096)
	 */
	// 我们预留了8个页面（8 x 4096）来存储页面结构。
	// 这应该足够管理最多128 MB（8 x 4096 x 4096）的内存。
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;
	printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);

	// 初始化所有页面描述符为0
	struct Page *page = (struct Page *)HEAP_START;
	for (int i = 0; i < _num_pages; i++)
	{
		_clear(page);
		page->malloc_flag = 0;
		page->_start = _align_page(HEAP_START + i * PAGE_SIZE);
		page->_end = page->_start + PAGE_SIZE;
		// if (i < 10)
		// 	printf("page[%d] = %x --> %x\n", i, page->_start, page->_end);
		page++;
	}

	// 计算堆池的起始地址和结束地址
	_alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE);
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);
	printf("page_init  alloc:%x-->%x\n", _alloc_start, _alloc_end);

	// 打印不同内存段的起始地址和结束地址
	printf("TEXT:   0x%x -> 0x%x\n", TEXT_START, TEXT_END);
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("DATA:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf("BSS:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}

/*
 * 分配由连续物理页面组成的内存块
 * - npages: 分配的页面数
 */
void *page_alloc(int npages)
{
	/* 注意我们在搜索页面描述符位图。 */
	int found = 0;
	struct Page *page_i = (struct Page *)HEAP_START;
	for (int i = 0; i <= (_num_pages - npages); i++)
	{
		if (_is_free(page_i))
		{
			found = 1;
			/*
			 * 遇到一个空闲页面，继续检查接下来的
			 * （npages-1）个页面是否也未被分配。
			 */
			struct Page *page_j = page_i + 1;
			for (int j = i + 1; j < (i + npages); j++)
			{
				if (!_is_free(page_j))
				{
					found = 0;
					break;
				}
				page_j++;
			}
			/*
			 * 获得足够好的内存块，
			 * 进行管理，然后返回该内存块的实际起始地址，即第一页的地址。
			 */
			if (found)
			{
				struct Page *page_k = page_i;
				for (int k = i; k < (i + npages); k++)
				{
					_set_flag(page_k, PAGE_TAKEN);
					page_k->malloc_flag = 1;
					page_k++;
					// printf("page_alloc  %x\n", page_k->_start);
				}
				page_k--;
				_set_flag(page_k, PAGE_LAST);
				return (void *)(_alloc_start + i * PAGE_SIZE);
			}
		}
		page_i++;
	}
	return NULL;
}

/*
 * 释放内存块
 * - p: 内存块的起始地址
 */
void page_free(void *p)
{
	/*
	 * 断言（TBD）p是否无效
	 */
	if (!p || (uint32_t)p >= _alloc_end)
	{
		return;
	}
	/* 获取该内存块的第一个页面描述符 */
	struct Page *page = (struct Page *)HEAP_START;
	page += ((uint32_t)p - _alloc_start) / PAGE_SIZE;
	printf("page_free %x\n", page->_start);

	/* 循环并清除该内存块的所有页面描述符 */
	while (!_is_free(page))
	{
		if (_is_last(page))
		{
			_clear(page);
			break;
		}
		else
		{
			_clear(page);
			page++;
		}
	}
}

void *malloc(size_t size)
{
	int found = 0, temp = 0;
	struct Page *page_i = (struct Page *)HEAP_START;
	struct Page *page_j, *page_k;
	int npages = size / PAGE_SIZE;
	uint32_t start_address;
	// 如果要分配大小正好是整数个页，调用page_alloc
	if (npages * PAGE_SIZE == size)
		return page_alloc(npages);
	npages++;
	for (int i = 0; i <= (_num_pages - npages); i++)
	{
		temp = 0;
		// printf("page[%d] : %x-->%x     size = %x\n", i, page_i->_start, page_i->_end, size);
		// 查找有剩余空间的页
		if (page_i->malloc_flag == 0)
		{
			if (page_i->_end - page_i->_start <= (uint32_t)(size - (size / PAGE_SIZE) * PAGE_SIZE))
			{
				temp = 1;
				// printf("remain %x\n", (uint32_t)(size - (size / PAGE_SIZE) * PAGE_SIZE));
			}
			found = 1;
			page_j = page_i + 1;
			for (int j = i + 1; j < i + npages + temp; j++)
			{
				if (!_is_free(page_j))
				{
					found = 0;
					break;
				}
				page_j++;
			}
			// 查找到足够的内存
			if (found)
			{
				page_k = page_i;
				start_address = page_i->_start;
				for (int k = i; k < i + npages + temp; k++)
				{
					if ((uint32_t)size >= page_k->_end - page_k->_start)
					{
						size -= ((size_t)(page_k->_end - page_k->_start));
						page_k->malloc_flag = 1;
						_set_flag(page_k, PAGE_TAKEN);
					}
					else
					{
						page_k->_start = page_k->_start + (uint32_t)size;
						_set_flag(page_k, PAGE_TAKEN);
						_set_flag(page_k, PAGE_LAST);
						printf("malloc: start->% x, end->% x\n", page_k->_start, page_k->_end);
						// return (void *)(page_i->_start);
						return (void *)(start_address + 8 * PAGE_SIZE);
					}
					page_k++;
				}
			}
		}
		page_i++;
	}
	return NULL;
}

void free(void *ptr)
{
	if (!ptr || (uint32_t)ptr >= _alloc_end)
	{
		return;
	}

	struct Page *page = (struct Page *)HEAP_START;
	page += (int)(((uint32_t)ptr - _alloc_start) / PAGE_SIZE);
	printf("free %x\n", page->_start);
	while (!_is_free(page))
	{
		if (_is_last(page))
		{
			// page->_start = _align_page((page - (struct Page *)HEAP_START) * PAGE_SIZE + HEAP_START);
			page->_start = (uint32_t)ptr - 8 * PAGE_SIZE;
			// printf("free page->start %x\n", page->_start);
			_clear(page);
			break;
		}
		else
		{
			page->_start = (uint32_t)ptr - 8 * PAGE_SIZE;
			// printf("free page->start %x\n", page->_start);
			_clear(page);
			page->malloc_flag = 0;
			page++;
		}
	}
}
void page_test()
{
	// void *p1 = page_alloc(2);
	// printf("p1 = 0x%x\n", p1);
	// page_free(p1);

	// void *p2 = page_alloc(7);
	// printf("p2 = 0x%x\n", p2);
	// page_free(p2);

	void *p3 = malloc(8);
	printf("p3 = 0x%x\n", p3);
	// free(p3);

	void *p4 = malloc(44);
	printf("p4 = 0x%x\n", p4);
	free(p4);
}