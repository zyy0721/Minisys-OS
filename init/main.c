/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <inc/printf.h>
#include <inc/pmap.h>
#include <kern/console.h>
#include <kern/fs.h>

#include <drivers/seven_seg.h>
#include <drivers/timer.h>
#include <drivers/leds.h>

#include <mips/cpu.h>

void print_banner();

void __attribute__ ((interrupt, keep_interrupts_masked)) _mips_general_exception ()
{
    unsigned cause = mips32_getcr ();  // Coprocessor 0 Cause register

    // check cause of interrupt
    if ((cause & 0x7c) != 0) {
        set_leds(0x8001);  // Display 0x8001 on LEDs to indicate exception state
        set_seven_seg_value(cause); // Display CR to seven_seg
        printf("cause number is %x\n", cause);
        printf("IP0: %d\n", cause & 0x00000100);
        printf("IP1: %d\n", cause & 0x00000200);
        printf("IP2: %d\n", cause & 0x00000400);
        printf("IP3: %d\n", cause & 0x00000800);
        printf("IP4: %d\n", cause & 0x00001000);
        printf("IP5: %d\n", cause & 0x00002000);
        printf("IP6: %d\n", cause & 0x00004000);
        printf("IP7: %d\n", cause & 0x00008000);
        printf("Exc Code: %d\n", (cause & 0x0000007C) >> 2);
        while (1);  // Loop forever non-interrupt exception detected
    }
    if (cause & CR_HINT0) {
        // axi timer interrupt
       // cprintf("TIMER INT: %d\n", TIMER_INT_CNT++);
        sched_yield();
        clear_timer0_int();
    } else if (cause & CR_HINT4) {
        // axi uart interrupt
        printf("UART INT: %c\n", get_UART_RBR());
    } else {
        set_seven_seg_value(cause); // Display CR
    }
}

int main()
{
    set_leds(0x5555); // let leds show 0x5555 when startup

    // set up interrupts
    // Clear boot interrupt vector bit in Coprocessor 0 Status register
    mips32_bicsr (SR_BEV);

    // Set master interrupt enable bit, as well as individual interrupt
    // enable bits in Coprocessor 0 Status register
    mips32_bissr (SR_IE | SR_HINT0 | SR_HINT1 | SR_HINT2 | SR_HINT3 | SR_HINT4);

    // Enable interrupt
    // Can't get interrupt until after we do this
    asm ("ei");

    // Initialize the console.
    // Can't call printf until after we do this!
    cons_init();
    // Print banner when start up.
    print_banner();

    // Enable seven segment digital tubes
    init_seven_seg();
    enable_all_seven_seg();

    // Initialize the file system
    fs_init();


    // Enable timer
    // init_timer();

	printf("main.c:\tmain is start ...\n");

	mips_init();
	panic("main is over is error!");

	return 0;
}

void print_banner() {
    printf("\n");
    printf("\n");
    printf("\n");

    printf("    __  ____       _                    ____  _____\n");
    printf("   /  |/  (_)___  (_)______  _______   / __ \\/ ___/\n");
    printf("  / /|_/ / / __ \\/ / ___/ / / / ___/  / / / /\\__ \\ \n");
    printf(" / /  / / / / / / (__  ) /_/ (__  )  / /_/ /___/ / \n");
    printf("/_/  /_/_/_/ /_/_/____/\\__, /____/   \\____//____/  \n");
    printf("                      /____/                       \n");

    printf("\n");
}
