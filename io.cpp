#include <stdint.h>
#include <stddef.h>

#include <machine.h>
#include <memory.h>
#include <debugging.h>

#include "banked_memory.h"
#include "console.h"
#include "io.h"
#include "disk.h"

void IO::reset() {
	_console.reset();
	_disk.reset();

	// read boot sector
	_disk.select(0);
	_disk.track(0);
	_disk.sector(1);
	_disk.dma(0);
	_disk.seek();
	_disk.read(_mem);
}

uint8_t IO::clk_data() {

	uint32_t s = _machine->microseconds() / 1000000;

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
		DBG_EMU("IO: unhandled clk_data(%u)", clkcmd);
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
		return _console.available();
	case CON_IN:
		return _console.poll();
	case FDC_STATUS:
		return _disk.status();
	case FDC_IODONE:
		return 1;
	case FDC_GETSEC_L:
		return _disk.sector() & 0xff;
	case FDC_GETSEC_H:
		return (_disk.sector() & 0xff00) >> 8;
	case FDC_GETTRK:
		return _disk.track();
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
		return timer >= 0? 1: 0;
	case CON1_ST:
	case CON2_ST:
	case NET1_ST:
		return 0x00;	// ignore
	default:
		DBG_EMU("IO: unhandled IN(%u)", port);
		break;
	}
	return 0x00;
}

void IO::out(uint16_t port, uint8_t a) {

	port &= 0xff;

	switch(port) {
	case FDC_SELDSK:
		_disk.select(a);
		break;
	case FDC_SETTRK:
		_disk.track(a);
		break;
	case FDC_SETSEC_L:
		_disk.sector((_disk.sector() & 0xff00) | a);
		break;
	case FDC_SETSEC_H:
		_disk.sector(a << 8 | (_disk.sector() & 0xff));
		break;
	case FDC_SETDMA_L:
		_disk.dma((_disk.dma() & 0xff00) | a);
		break;
	case FDC_SETDMA_H:
		_disk.dma((a << 8) | (_disk.dma() & 0xff));
		break;
	case FDC_IO:
		a? _disk.write(_mem): _disk.read(_mem);
		break;
	case CON_OUT:
		_console.write(a);
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
		if (timer >= 0 && !a) {
			_machine->cancel_timer(timer);
			timer = -1;
		} else if (timer < 0 && a && tick_handler)
			timer = _machine->interval_timer(10000, tick_handler);
		break;
	case CLK_CMD:
		clk_cmd(a);
		break;
	case MONITOR:
		ERR("IO: monitor(%02x)", a);
		break;
	default:
		DBG_EMU("IO: unhandled OUT(%u, %u)", port, a);
		break;
	}
}
