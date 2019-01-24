#include "leds.h"

#include <mips/cpu.h>
#include <inc/mfp_io.h>

void set_leds(u32 v) { mips_put_word(LEDS_ADDR, v); }
