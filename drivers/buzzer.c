#include "buzzer.h"

#include <mips/cpu.h>
#include <inc/mfp_io.h>

void set_buzzers(u32 v)
{
    mips_put_word(BUZZER_ADDR,v);
}
void delay_zero()
{
    volatile unsigned int j = 0;
    for (; j < 2500000; j++);

}
void delay_do()
{
    set_leds(0x111111);
    set_seven_seg_value(1048);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(1048);
    set_buzzers(0);
}
void delay_re()
{
    set_leds(0x0222222);
    set_seven_seg_value(988);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(988);
    set_buzzers(0);
}
void delay_mi()
{
    set_leds(0x333333);
    set_seven_seg_value(880);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(880);
    set_buzzers(0);
}
void delay_long_mi()
{
    set_leds(0x333333);
    set_seven_seg_value(880);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(880);
    set_buzzers(0);
}
void delay_fa()
{
    set_leds(0x444444);
    set_seven_seg_value(784);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(784);
    set_buzzers(0);
}
void delay_so()
{
    set_leds(0x555555);
    set_seven_seg_value(698);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(698);
    set_buzzers(0);
}
void delay_la()
{
    set_leds(0x666666);
    set_seven_seg_value(660);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(660);
    set_buzzers(0);
}
void delay_xi()
{
    set_leds(0x777777);
    set_seven_seg_value(588);
    volatile unsigned int j = 0;
    for (; j < 1000000; j++)
        set_buzzers(588);
    set_buzzers(0);
}

//0 
//1 262 0x00000106
//2 294 0x00000126
//3 330 0x0000014A
//4 349 0x0000015D
//5 392 0x00000188
//6 440 0x000001B8
//7 494 0x000001EE

void start_ringtone()
{
    //SONG OF JOY
    delay_xi();
    delay_zero();
    delay_mi();
    delay_mi();
    delay_fa();
    delay_so();
    delay_zero();
    delay_so();
    delay_fa();
    delay_mi();
    delay_re();
    delay_zero();
    delay_do();
    delay_do();
    delay_re();
    delay_mi();
    delay_zero();
    delay_long_mi();
    delay_re();
    delay_re();
}
void two_tigers()
{
    delay_xi();
    delay_zero();

    delay_do();
    delay_re();
    delay_mi();
    delay_do();
    delay_zero();
    delay_do();
    delay_re();
    delay_mi();
    delay_do();
    delay_zero();

    delay_mi();
    delay_fa();
    delay_so();
    delay_zero();
    delay_mi();
    delay_fa();
    delay_so();
    set_seven_seg_value(0);
    set_leds(0x5555);

}