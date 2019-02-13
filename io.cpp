#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <CPU.h>
#include <ports.h>
#include <i8080.h>
#include <tftdisplay.h>

#include "io.h"

void IO::reset() {
	dsk_reset();
	kbd_reset();
	scr_reset();
}

uint8_t IO::in(uint16_t port, i8080 *cpu) {
	uint8_t c = 0;
	port &= 0xff;
	if (port == 4)
		c = kbd_read();
	else if (port == 2)
		c = kbd_avail();
	else if (port == 14)
		c = dsk_read();
	else if (port == 15)
		c = dsk_write();
	return c;
}

void IO::out(uint16_t port, uint8_t a, i8080 *cpu) {
	port &= 0xff;
	if (port == 4)
		scr_display(a);
	else if (port == 20)
		dsk_select(a);
	else if (port == 21)
		settrk = a;
	else if (port == 22)
		setsec = a;
	else if (port == 23)
		setdma = cpu->hl();
}
