#include "timer.h"

#include <mips/cpu.h>
#include <inc/mfp_io.h>
#include <inc/printf.h>
#include <drivers/seven_seg.h>

#define AXI_CLOCK_PERIOD_HZ     50000000
#define AXI_CLOCK_PERIOD_KHZ    50000

// Register Address Map                // Description
#define TCSR0_ADDR (0x00 + TIMER_ADDR) // Timer 0 Control and Status Register
#define TLR0_ADDR  (0x04 + TIMER_ADDR) // Timer 0 Load Register
#define TCR0_ADDR  (0x08 + TIMER_ADDR) // Timer 0 Counter Register
#define TCSR1_ADDR (0x10 + TIMER_ADDR) // Timer 1 Control and Status Register
#define TLR1_ADDR  (0x14 + TIMER_ADDR) // Timer 1 Load Register
#define TCR1_ADDR  (0x18 + TIMER_ADDR) // Timer 1 Counter Register

#define TIMER_TCSR0_CASC    (1 << 11)   // Enable cascade mode of timers
#define TIMER_TCSR0_ENALL   (1 << 10)   // Enable All Timers
#define TIMER_TCSR0_PWMA0   (1 << 9)    // Enable Pulse Width Modulation for Timer 0
#define TIMER_TCSR0_T0INT   (1 << 8)    // Timer 0 Interrupt
#define TIMER_TCSR0_ENT0    (1 << 7)    // Enable Timer 0
#define TIMER_TCSR0_ENIT0   (1 << 6)    // Enable Interrupt for Timer 0
#define TIMER_TCSR0_LOAD0   (1 << 5)    // Load Timer 0
#define TIMER_TCSR0_ARHT0   (1 << 4)    // Auto Reload/Hold Timer 0
#define TIMER_TCSR0_CAPT0   (1 << 3)    // Enable External Capture Trigger Timer 0
#define TIMER_TCSR0_GENT0   (1 << 2)    // Enable External Generate Signal Timer 0
#define TIMER_TCSR0_UDT0    (1 << 1)    // Up/Down Count Timer 0
#define TIMER_TCSR0_MDT0    (1 << 0)    // Timer mode is generate

void set_TCSR0(u32 v) { mips_put_word(TCSR0_ADDR, v); }
u32 get_TCSR0() { return mips_get_word(TCSR0_ADDR, NULL); }
void set_TLR0(u32 v) { mips_put_word(TLR0_ADDR, v); }
u32 get_TLR0() { return mips_get_word(TLR0_ADDR, NULL); }
u32 get_TCR0() { return mips_get_word(TCR0_ADDR, NULL); }

bool get_timer0_int() { return (get_TCSR0() & TIMER_TCSR0_T0INT) != 0; }
void clear_timer0_int() { set_TCSR0(get_TCSR0() | TIMER_TCSR0_T0INT); }

void enable_timer0() { set_TCSR0(get_TCSR0() | TIMER_TCSR0_ENT0); }
void disable_timer0() { set_TCSR0(get_TCSR0() & ~(TIMER_TCSR0_ENT0)); }

void set_timing_interval_s(u32 s) {
    if (s > 85) {
        panic("Timeing interval cannot be over 85 seconds.\n");
        return;
    }
    set_TLR0(s * AXI_CLOCK_PERIOD_HZ - 4);
}

void set_timing_interval_ms(u32 ms) {
    if (ms > 85899) {
        panic("Timeing interval cannot be over 85899 milliseconds.\n");
        return;
    }
    set_TLR0(ms * AXI_CLOCK_PERIOD_KHZ - 4);
}

void init_timer() {
    set_TCSR0(0);
    set_timing_interval_s(1);
    set_TCSR0(
        TIMER_TCSR0_ENIT0 |
        TIMER_TCSR0_ARHT0 |
        TIMER_TCSR0_UDT0
    );
    enable_timer0();
}