#include <Energia.h>
#include <UTFT.h>
#include <SD.h>
#include <r65emu.h>
#include <i8080.h>

#include "io.h"

void IO::reset() {
	dsk_reset();
	kbd_reset();
	scr_reset();
}

byte IO::in(byte port, i8080 *cpu) {
	byte c = 0;
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

void IO::out(byte port, byte a, i8080 *cpu) {
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
