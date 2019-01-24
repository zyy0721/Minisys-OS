#pragma once

#include <inc/types.h>

u32 get_seven_seg_enable();
void set_seven_seg_enable(u32 v);
u32 get_seven_seg_value();
void set_seven_seg_value(u32 v);

void init_seven_seg();

void enable_one_seven_seg(u32 pos);

void enable_all_seven_seg();

void disable_one_seven_seg(u32 pos);

void disable_all_seven_seg();

void set_seven_seg_digit(u32 val, u32 pos);

void clean_seven_seg_value();
