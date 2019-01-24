#pragma once

#include <inc/mfp_io.h>
#include <inc/types.h>

// Interrupt Enable Register Bit Definitions
#define EDSSI (1 << 3) // Enable Modem Status Interrupt.
#define ELSI  (1 << 2) // Enable Receiver Line Status Interrupt.
#define ETBEI (1 << 1) // Enable Transmitter Holding Register Empty Interrupt.
#define ERBFI (1 << 0) // Enable Received Data Available Interrupt.

// Interrupt Identification Register Bit Definitions
#define MODE_16450 (0 << 6) // FIFOs Enabled. Always zero if not in FIFO mode. 0 - 16450 mode,
#define MODE_16550 (1 << 6) // 1 - 16550 mode.

#define RLS  (0b011 << 1) // Receiver Line Status (Highest).
#define RDA  (0b010 << 1) // Received Data Available (Second).
#define CT   (0b110 << 1) // Character Timeout (Second).
#define THRE (0b001 << 1) // Transmitter Holding Register Empty (Third).
#define MS   (0b000 << 1) // Modem Status (Fourth).

#define INTPEND    (0 << 0) // Interrupt Pending : 0 - Interrupt is pending,
#define NO_INTPEND (1 << 0) // 1 - No interrupt is pending.

// Line Control Register Bit Definitions
#define DLAB            (1 << 7) // Divisor Latch Access Bit.
#define SET_BREAK       (1 << 6) // Set Break.
#define STICK_PARITY    (1 << 5) // Stick Parity.
#define EVEN_PARITY     (1 << 4) // Selects Even parity.
#define ODD_PARITY      (0 << 4) // Selects Odd parity.
#define PEN             (1 << 3) // Parity Enable.
#define STOP_BITS_1     (0 << 2) // 1 Stop bit;
#define STOP_BITS_2     (1 << 2) // 2 Stop bits or 1.5, if 5 bits/character selected.
#define BITS_5_PER_CHAR (0b00) // 5 bits/character.
#define BITS_6_PER_CHAR (0b01) // 6 bits/character.
#define BITS_7_PER_CHAR (0b10) // 7 bits/character.
#define BITS_8_PER_CHAR (0b11) // 8 bits/character.

// Line Status Register Bit Definitions
bool get_UART_ERF(u32 v);  // Error in RCVR FIFO (1)
bool get_UART_TEMT(u32 v); // Transmitter Empty
bool get_UART_THRE(u32 v); // Transmitter Holding Register Empty.
bool get_UART_BI(u32 v);   // Break Interrupt.
bool get_UART_FE(u32 v);   // Framing Error.
bool get_UART_PE(u32 v);   // Parity Error.
bool get_UART_OE(u32 v);   // Overrun Error.
bool get_UART_DR(u32 v);   // Data Ready.

u32 get_UART_RBR();
void set_UART_THR(u32 v);
u32 get_UART_IER();
void set_UART_IER(u32 v);
u32 get_UART_IIR();
void set_UART_FCR(u32 v);
u32 get_UART_FCR();
void set_UART_LCR(u32 v);
u32 get_UART_LCR();
void set_UART_MCR(u32 v);
u32 get_UART_MCR();
void set_UART_LSR(u32 v);
u32 get_UART_LSR();
void set_UART_MSR(u32 v);
u32 get_UART_MSR();
void set_UART_SCR(u32 v);
u32 get_UART_SCR();
void set_UART_DLL(u32 v);
u32 get_UART_DLL();
void set_UART_DLM(u32 v);
u32 get_UART_DLM();

void init_uart();
