#include "seven_seg.h"

#include <mips/cpu.h>
#include <inc/mfp_io.h>

u32 get_seven_seg_enable() { return mips_get_word(SEVEN_SEG_EN_ADDR, NULL); }
void set_seven_seg_enable(u32 v) { mips_put_word(SEVEN_SEG_EN_ADDR, v); }
u32 get_seven_seg_value() { return mips_get_word(SEVEN_SEG_ADDR, NULL); }
void set_seven_seg_value(u32 v) { mips_put_word(SEVEN_SEG_ADDR, v); }

void init_seven_seg() {
    disable_all_seven_seg();
    clean_seven_seg_value();
}

void enable_one_seven_seg(u32 pos) {
    if (pos < 8)
        set_seven_seg_enable(get_seven_seg_enable() | 1 << pos);
}

void enable_all_seven_seg() { set_seven_seg_enable(0xff); }

void disable_one_seven_seg(u32 pos) {
    if (pos < 8)
        set_seven_seg_enable(get_seven_seg_enable() & ~(1 << pos));
}

void disable_all_seven_seg() { set_seven_seg_enable(0); }

void set_seven_seg_digit(u32 val, u32 pos) {
    if (val < 16 && pos < 8)
        set_seven_seg_value(get_seven_seg_enable() & ~(0xf << pos * 4) | val << pos * 4);
}

void clean_seven_seg_value() { set_seven_seg_value(0); }