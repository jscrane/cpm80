#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <CPU.h>
#include <display.h>
#include <serial_kbd.h>
#include <serial_dsp.h>
#include <hardware.h>
#include <debugging.h>

#include "banked_memory.h"
#include "screen.h"
#include "io.h"

void IO::reset() {
	_kbd.reset();
	_dsp.reset();
	dsk_reset();
}

uint8_t IO::kbd_poll() {
	uint8_t c;
	do {
#if !defined(Energia_h)
		yield();
#endif
		c = _kbd.read();
	} while (c == 0xff);
	return c;
}

uint8_t IO::clk_data() {

	uint32_t s = millis() / 1000;

	switch (clkcmd) {
	case 0:
		return s % 60;

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		DBG_EMU(printf("IO: unhandled clk_data(%u)\r\n", clkcmd));
		break;
	}
	return 0x00;
}

void IO::clk_cmd(uint8_t cmd) {

	clkcmd = cmd;

	if (cmd == 0xff)
		clkfmt = !clkfmt;
}

uint8_t IO::in(uint16_t port) {

	port &= 0xff;

	switch (port) {
	case CON_ST:
		return _kbd.available()? 0xff: 0x00;
	case CON_IN:
		return kbd_poll();
	case FDC_STATUS:
		return dsk_status;
	case FDC_IODONE:
		return 1;
	case FDC_GETSEC_L:
		return setsec & 0xff;
	case FDC_GETSEC_H:
		return (setsec & 0xff00) >> 8;
	case FDC_GETTRK:
		return settrk;
	case MEM_INIT:
		return _mem.num_banks();
	case MEM_SELECT:
		return _mem.selected();
	case MEM_BANKSIZE:
		return _mem.bank_size();
	case MEM_WP_COMMON:
		return _mem.wp_common();
	case CLK_DATA:
		return clk_data();
	case CLK_CMD:
		return clkfmt;
	case TIMER:
		return timer? 1: 0;
	case CON1_ST:
	case CON2_ST:
	case NET1_ST:
		return 0x00;	// ignore
	default:
		DBG_EMU(printf("IO: unhandled IN(%u)\r\n", port));
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
		dsk_status = dsk_setsec((setsec & 0xff00) | a);
		break;
	case FDC_SETSEC_H:
		dsk_status = dsk_setsec(a << 8 | (setsec & 0xff));
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
	case MEM_INIT:
		_mem.begin(a);
		break;
	case MEM_SELECT:
		_mem.select(a);
		break;
	case MEM_BANKSIZE:
		_mem.bank_size(a);
		break;
	case MEM_WP_COMMON:
		_mem.wp_common(a);
		break;
	case TIMER:
		if (timer && !a) {
			hardware_cancel_timer(timer);
			timer = 0;
		} else if (!timer && a && tick_handler)
			timer = hardware_interval_timer(10, tick_handler);
		break;
	case CLK_CMD:
		clk_cmd(a);
		break;
	case MONITOR:
		// FIXME: reboot is 0x01, monitor is 0x80
		break;
	default:
		DBG_EMU(printf("IO: unhandled OUT(%u, %u)\r\n", port, a));
		break;
	}
}
