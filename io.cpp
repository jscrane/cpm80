#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <CPU.h>
#include <ports.h>
#include <display.h>
#include <serial_kbd.h>
#include <serial_dsp.h>
#include <hardware.h>

#include "screen.h"
#include "io.h"

void IO::reset() {
	_kbd.reset();
	_dsp.reset();
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

	port &= 0xff;

	switch(port) {
	case CON_ST:
		return _kbd.available()? 0xff: 0x00;
	case CON_IN:
		return kbd_poll();
	case FDC_STATUS:
		return dsk_status;
	case FDC_IODONE:
		return 1;
	case FDC_GETSEC_L:
		return setsec;
	case FDC_GETTRK:
		return settrk;
	default:
		DBG(printf("IO: unhandled IN(%u)\r\n", port));
		break;
	}
	return 0x00;
}

void IO::out(uint16_t port, uint8_t a) {

	port &= 0xff;

	switch(port) {
	case FDC_SELDSK:
		dsk_status = dsk_select(a);
		break;
	case FDC_SETTRK:
		dsk_status = dsk_settrk(a);
		break;
	case FDC_SETSEC_L:
		dsk_status = dsk_setsec(a);
		break;
	case FDC_SETDMA_L:
		setdma = (setdma & 0xff00) | a;
		break;
	case FDC_SETDMA_H:
		setdma = (a << 8) | (setdma & 0xff);
		break;
	case FDC_IO:
		dsk_status = (a? dsk_write(): dsk_read());
		break;
	case CON_OUT:
		_dsp.write(a);
		break;
	case FDC_SETSEC_H:
		// ignore?
		break;
	default:
		DBG(printf("IO: unhandled OUT(%u, %u)\r\n", port, a));
		break;
	}
}
