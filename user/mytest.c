#include "lib.h"
#include <mips/cpu.h>

void umain() {
    mips_put_word(0xB0600000, 0x12321);
    while(1);
}