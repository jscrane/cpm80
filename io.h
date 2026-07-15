#pragma once

// input ports: A = IN(n)
// (see https://github.com/udo-munk/z80pack/blob/master/cpmsim/srcsim/simio.c)
#define CON_ST		0
#define CON_IN		1
#define FDC_GETTRK	11
#define FDC_GETSEC_L	12
#define FDC_IODONE	13
#define FDC_STATUS	14
#define FDC_GETSEC_H	17
#define CLK_DATA	26
#define CON1_ST		40
#define CON2_ST		42
#define NET1_ST		50

// output ports: OUT(n, A)
#define CON_OUT		1
#define FDC_SELDSK	10
#define FDC_SETTRK	11
#define FDC_SETSEC_L	12
#define FDC_IO		13
#define FDC_SETDMA_L	15
#define FDC_SETDMA_H	16
#define FDC_SETSEC_H	17
#define MEM_INIT	20
#define MEM_SELECT	21
#define MEM_BANKSIZE	22
#define MEM_WP_COMMON	23
#define CLK_CMD		25
#define TIMER		27
#define MONITOR		29

class BankedMemory;
class Console;
class Disk;

class IO {
public:
	IO(BankedMemory &mem, Console &console, Disk &disk): _console(console), _disk(disk), _mem(mem) {}

	uint8_t in(uint16_t p);
	void out(uint16_t p, uint8_t b);

	void reset();
	void register_timer_interrupt_handler(std::function<void(void)> fn) { tick_handler = fn; }

private:
	Console &_console;
	Disk &_disk;

	std::function<void(void)> tick_handler;
	int8_t timer = -1;

	uint8_t clk_data();
	void clk_cmd(uint8_t);
	uint8_t clkfmt, clkcmd;

	BankedMemory &_mem;
};
