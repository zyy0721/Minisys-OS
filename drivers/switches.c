#include "switches.h"

#include <mips/cpu.h>
#include <inc/mfp_io.h>

u32 get_switches() { return mips_get_word(SWITCHES_ADDR, NULL); }
