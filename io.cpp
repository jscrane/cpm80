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
	_kbd.reset();
	scr_reset();
	dsk_reset();
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
	case CON_ST:
		return _kbd.available()? 0xff: 0x00;
	case CON_IN:
		return kbd_poll();
	case FDC_IN:
		return dsk_read();
	case FDC_OUT:
		return dsk_write();
	default:
		DBG(printf("IO: unhandled input port: %x\r\n", port));
	}
	return 0x00;
}

void IO::out(uint16_t port, uint8_t a, i8080 *cpu) {

	switch(port & 0xff) {
	case CON_OUT:
		scr_display(a);
		break;
	case FDC_SELDSK:
		dsk_select(a);
		break;
	case FDC_SETTRK:
		settrk = a;
		break;
	case FDC_SETSEC:
		setsec = a;
		break;
	case FDC_SETDMA:
		setdma = cpu->hl();
		break;
	default:
		DBG(printf("IO: unhandled output port: %x\r\n", port));
	}
}
