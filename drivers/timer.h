#pragma once

#include <inc/types.h>

void set_TCSR0(u32 v);
u32 get_TCSR0();
void set_TLR0(u32 v);
u32 get_TCR0();

bool get_timer0_int();
void clear_timer0_int();

void enable_timer0();
void disable_timer0();

void set_timing_interval_s(u32 s);
void set_timing_interval_ms(u32 ms);

void init_timer();