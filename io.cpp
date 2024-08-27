#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <CPU.h>
#include <ports.h>
#include <display.h>
#include <hardware.h>

#include "config.h"
#include PROCESSOR_H
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

uint8_t IO::in(uint16_t port) {

	switch(port & 0xff) {
	case CON_ST:
		return _kbd.available()? 0xff: 0x00;
	case CON_IN:
		return kbd_poll();
	case FDC_STATUS:
		return dsk_status;
	case FDC_IODONE:
		return 1;
	default:
		DBG(printf("IO: unhandled IN(%u)\r\n", port));
		break;
	}
	return 0x00;
}

void IO::out(uint16_t port, uint8_t a) {

	switch(port & 0xff) {
	case FDC_SELDSK:
		dsk_status = dsk_select(a);
		break;
	case FDC_SETTRK:
		dsk_status = dsk_settrk(a);
		break;
	case FDC_SETSEC:
		dsk_status = dsk_setsec(a);
		break;
	case FDC_SETDMA_L:
		setdma = a;
		dsk_status = OK;
		break;
	case FDC_SETDMA_H:
		setdma |= (a << 8);
		dsk_status = OK;
		break;
	case FDC_IO:
		dsk_status = (a? dsk_write(): dsk_read());
		break;
	case CON_OUT:
		scr_display(a);
		break;
	default:
		DBG(printf("IO: unhandled OUT(%u)\r\n", port));
		break;
	}
}
