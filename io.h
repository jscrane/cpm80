#ifndef __IO_H__
#define __IO_H__

class serial_kbd;

// input ports (see CBIOS.ASM)
#define CON_ST	2
#define CON_IN	4
#define FDC_IN	14
#define FDC_OUT	15

// output ports
#define CON_OUT		4
#define FDC_SELDSK	20
#define FDC_SETTRK	21
#define FDC_SETSEC	22
#define FDC_SETDMA	23

class IO: public PortDevice<i8080>, public Display {
public:
	IO(Memory &mem, serial_kbd &kbd): _mem(mem), _kbd(kbd) {}

	uint8_t in(uint16_t p, i8080 *cpu);
	void out(uint16_t p, uint8_t b, i8080 *cpu);

	void reset();
private:
	serial_kbd &_kbd;
	bool _brk;
	uint8_t kbd_poll();

	void dsk_reset();
	uint8_t dsk_read();
	uint8_t dsk_write();
	void dsk_seek();
	void dsk_select(uint8_t a);
	uint8_t settrk, setsec, trk, sec;
	uint16_t setdma;
	Memory &_mem;

	void dsk_led(unsigned colour = 0x0000);

	void scr_reset();
	void scr_clear();
	void scr_draw(char, unsigned, unsigned);
	void scr_display(uint8_t);
	bool _esc, _ansi;
	unsigned _value, _line;
};

#endif
