#include <stdio.h>

int foo(int a, int b)
{
	//int c = a + b;
	int c;
	asm volatile(
		"add %0,%1,%2\n"
		:"=r"(c)
		:"r"(a),"r"(b)		
	);
	return c;
}
int main()
{
	printf("foo(1,2) -> %d\n",foo(1,2));
	return 0;
}
