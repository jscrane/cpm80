#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <CPU.h>
#include <ports.h>
#include <i8080.h>
#include <display.h>
#include <hardware.h>

#include "serial_kbd.h"
#include "io.h"

void IO::reset() {
	dsk_reset();
	_kbd.reset();
	scr_reset();
	_brk = true;
}

uint8_t IO::kbd_poll() {
	uint8_t c;
	do {
#if !defined(Energia_h)
		yield();
#endif
		c = _kbd.read();
	} while (c == 0xff && !_brk);
	_brk = false;
	return c;
}

uint8_t IO::in(uint16_t port, i8080 *cpu) {

	switch(port & 0xff) {
	case 4:
		return kbd_poll();
	case 2:
		return _kbd.available()? 0xff: 0x00;
	case 14:
		return dsk_read();
	case 15:
		return dsk_write();
	default:
		DBG(printf("IO: unhandled input port: %x\r\n", port));
	}
	return 0x00;
}

void IO::out(uint16_t port, uint8_t a, i8080 *cpu) {

	switch(port & 0xff) {
	case 4:
		scr_display(a);
		break;
	case 20:
		dsk_select(a);
		break;
	case 21:
		settrk = a;
		break;
	case 22:
		setsec = a;
		break;
	case 23:
		setdma = cpu->hl();
		break;
	default:
		DBG(printf("IO: unhandled output port: %x\r\n", port));
	}
}
