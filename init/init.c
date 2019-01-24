#include <inc/asm/asm.h>
#include <inc/pmap.h>
#include <inc/env.h>
#include <inc/printf.h>
#include <kern/monitor.h>
#include <inc/traps.h>
#include <inc/tlbop.h>
#include <drivers/leds.h>
#include <drivers/buzzer.h>

void mips_init()
{
	two_tigers();
	printf("init.c:\tmips_init() is called\n");
	mips_tlbinvalall ();
	// Lab 2 memory management initial 0x401000,ization functions
	mips_detect_memory();
	mips_vm_init();
	page_init();
    page_check();

	env_init();
	env_check();
    //printf("env init sucessed !\n");	
		
	//ENV_CREATE(user_A);
	//ENV_CREATE(user_B);
	//print_tlb();
	ENV_CREATE(mytest);
	//print_tlb();

	// printf("OK,envA and envB create success!\n");
	trap_init();
	printf("finish trap_init!\n");
	//kclock_init();
	printf("finish kclock_init!\n");


	while (1)
		monitor(NULL);

	panic("init.c:\tend of mips_init() reached!");
}

void ddelay() {
	volatile u32 j;
	for (j = 0; j < 1000000; ++j);
}

void bcopy(const void *src, void *dst, size_t len)
{
	void *max;

	max = dst + len;
	int iii = 0;
	// copy machine words while possible
	while (dst + 3 < max) {
		if (iii++ < 5) printf("bcopy dst: %x src: %x\n", dst, *(int *)src);
		*(int *)dst = *(int *)src;
		dst += 4;
		src += 4;
	}

	// finish remaining 0-3 bytes
	while (dst < max) {
		*(char *)dst = *(char *)src;
		dst += 1;
		src += 1;
	}
}

void bzero(void *b, size_t len)
{
	void *max;

	max = b + len;

	//printf("init.c:\tzero from %x to %x\n",(int)b,(int)max);

	// zero machine words while possible

	while (b + 3 < max) {
		*(int *)b = 0;
		b += 4;
	}

	// finish remaining 0-3 bytes
	while (b < max) {
		*(char *)b++ = 0;
	}

}
