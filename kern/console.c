#include "console.h"

#include <drivers/uart.h>

static void cons_intr(int (*proc)(void));
static void cons_putc(int c);

/***** Serial I/O code *****/

static int serial_proc_data(void) {
    if (!get_UART_DR(get_UART_LSR())) // get when data is ready
        return -1;
    return get_UART_RBR();
}

void serial_intr(void) {
    cons_intr(serial_proc_data);
}

static void serial_putc(int c) {
    while (!get_UART_TEMT(get_UART_LSR()));
    set_UART_THR(c);
}

static void serial_init(void) {
    init_uart();
}

/***** General device-independent console code *****/
// Here we manage the console input buffer,
// where we stash characters received from the keyboard or serial port
// whenever the corresponding interrupt occurs.

#define CONSBUFSIZE 512

static struct {
    u8 buf[CONSBUFSIZE]; // buffer
    u32 rpos;            // read position
    u32 wpos;            // write position
} cons;

// called by device interrupt routines to feed input characters
// into the circular console input buffer.
static void cons_intr(int (*proc)(void)) {
    int c;
    while ((c = (*proc)()) != -1) {
        if (c == 0)
            continue;
        cons.buf[cons.wpos++] = c;
        if (cons.wpos == CONSBUFSIZE)
            cons.wpos = 0;
    }
}

// return the next input character from the console, or 0 if none waiting
int cons_getc(void) {
	int c;

	// poll for any pending input characters,
	// so that this function works even when interrupts are disabled
	// (e.g., when called from the kernel monitor).
	serial_intr();

	// grab the next character from the input buffer.
	if (cons.rpos != cons.wpos) {
		c = cons.buf[cons.rpos++];
		if (cons.rpos == CONSBUFSIZE)
			cons.rpos = 0;
		return c;
	}
	return 0;
}

// output a character to the console
static void cons_putc(int c) {
	if (c == '\n') {
		serial_putc(c);
		serial_putc('\r');
	} else
		serial_putc(c);
}

// initialize the console devices
void cons_init(void) {
	serial_init();
}

// `High'-level console I/O.  Used by readline and cprintf.

void cputchar(int c) {
	cons_putc(c);
}

int getchar(void) {
	int c;

	while ((c = cons_getc()) == 0)
		/* do nothing */;
	return c;
}

int iscons(int fdnum) {
	// used by readline
	return 1;
}
