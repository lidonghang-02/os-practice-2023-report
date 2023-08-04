#include <stdio.h>

int main()
{
	int num = 0x87654321;
	int low = num & 0xffff;
	int high = ( num >> 16 ) & 0xffff;
	
	printf("low 16 -> 0x%04X\n" ,low);
	printf("high 16 -> 0x%04X\n" ,high);
}
