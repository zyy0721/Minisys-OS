#include "uart.h"

#include <mips/cpu.h>

// Register Address Map                // LCR(7) Offset Name Access Description
#define  RBR_ADDR (0x1000 + UART_ADDR) // 0      0x1000 RBR  RO     Receiver Buffer Register
#define  THR_ADDR (0x1000 + UART_ADDR) // 0      0x1000 THR  WO     Transmitter Holding Register
#define  IER_ADDR (0x1004 + UART_ADDR) // 0      0x1004 IER  R/W    Interrupt Enable Register
#define  IIR_ADDR (0x1008 + UART_ADDR) // x      0x1008 IIR  RO     Interrupt Identification Register
#define  FCR_ADDR (0x1008 + UART_ADDR) // x      0x1008 FCR  WO     FIFO Control Register
                                       // 1      0x1008 FCR  RO     FIFO Control Register
#define  LCR_ADDR (0x100C + UART_ADDR) // x      0x100C LCR  R/W    Line Control Register
#define  MCR_ADDR (0x1010 + UART_ADDR) // x      0x1010 MCR  R/W    Modem Control Register
#define  LSR_ADDR (0x1014 + UART_ADDR) // x      0x1014 LSR  R/W    Line Status Register
#define  MSR_ADDR (0x1018 + UART_ADDR) // x      0x1018 MSR  R/W    Modem Status Register
#define  SCR_ADDR (0x101C + UART_ADDR) // x      0x101C SCR  R/W    Scratch Register
#define  DLL_ADDR (0x1000 + UART_ADDR) // 1      0x1000 DLL  R/W    Divisor Latch (Least Significant Byte) Register
#define  DLM_ADDR (0x1004 + UART_ADDR) // 1      0x1004 DLM  R/W    Divisor Latch (Most Significant Byte) Registe

/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/
u32 get_UART_RBR() { return mips_get_word(RBR_ADDR, NULL); }
void set_UART_THR(u32 v) { mips_put_word(THR_ADDR, v); }
u32 get_UART_IER() { return mips_get_word(IER_ADDR, NULL); }
void set_UART_IER(u32 v) { mips_put_word(IER_ADDR, v); }
u32 get_UART_IIR() { return mips_get_word(IIR_ADDR, NULL); }
void set_UART_FCR(u32 v) { mips_put_word(FCR_ADDR, v); }
u32 get_UART_FCR() { return mips_get_word(FCR_ADDR, NULL); }
void set_UART_LCR(u32 v) { mips_put_word(LCR_ADDR, v); }
u32 get_UART_LCR() { return mips_get_word(LCR_ADDR, NULL); }
void set_UART_MCR(u32 v) { mips_put_word(MCR_ADDR, v); }
u32 get_UART_MCR() { return mips_get_word(MCR_ADDR, NULL); }
void set_UART_LSR(u32 v) { mips_put_word(LSR_ADDR, v); }
u32 get_UART_LSR() { return mips_get_word(LSR_ADDR, NULL); }
void set_UART_MSR(u32 v) { mips_put_word(MSR_ADDR, v); }
u32 get_UART_MSR() { return mips_get_word(MSR_ADDR, NULL); }
void set_UART_SCR(u32 v) { mips_put_word(SCR_ADDR, v); }
u32 get_UART_SCR() { return mips_get_word(SCR_ADDR, NULL); }
void set_UART_DLL(u32 v) { mips_put_word(DLL_ADDR, v); }
u32 get_UART_DLL() { return mips_get_word(DLL_ADDR, NULL); }
void set_UART_DLM(u32 v) { mips_put_word(DLM_ADDR, v); }
u32 get_UART_DLM() { return mips_get_word(DLM_ADDR, NULL); }

bool get_UART_ERF(u32 v) { return (v & 1 << 7) != 0; }
bool get_UART_TEMT(u32 v) { return (v & 1 << 6) != 0; }
bool get_UART_THRE(u32 v) { return (v & 1 << 5) != 0; }
bool get_UART_BI(u32 v) { return (v & 1 << 4) != 0; }
bool get_UART_FE(u32 v) { return (v & 1 << 3) != 0; }
bool get_UART_PE(u32 v) { return (v & 1 << 2) != 0; }
bool get_UART_OE(u32 v) { return (v & 1 << 1) != 0; }
bool get_UART_DR(u32 v) { return (v & 1 << 0) != 0; }

void init_uart() {
    set_UART_LCR(DLAB); // LCR[7]  is 1
    set_UART_DLL(27); // DLL msb. 115200 at 50MHz. Formula is Clk/16/baudrate. From axi_uart manual.
    set_UART_DLM(0); // DLL lsb.
    set_UART_LCR(BITS_8_PER_CHAR | STOP_BITS_1); // LCR register. 8n1 parity disabled
    // set_UART_IER(ERBFI); // Enable Interrupts
    set_UART_IER(0); // Disable Interrupts
}
