#ifndef __IO_H__
#define __IO_H__

class serial_kbd;

// input ports: A = IN(n)
// (see https://st.sdf-eu.org/i8080/index.html)
#define CON_ST		0
#define CON_IN		1
#define FDC_IODONE	13
#define FDC_STATUS	14

// output ports: OUT(n, A)
#define CON_OUT		1
#define FDC_SELDSK	10
#define FDC_SETTRK	11
#define FDC_SETSEC	12
#define FDC_IO		13
#define FDC_SETDMA_L	15
#define FDC_SETDMA_H	16

// disk errors
#define OK		0
#define ILLEGAL_DRIVE	1
#define ILLEGAL_TRACK	2
#define ILLEGAL_SECTOR	3
#define SEEK_ERROR	4
#define READ_ERROR	5
#define WRITE_ERROR	6
#define ILLEGAL_CMD	7

class IO: public PortDevice<processor_t>, public Display {
public:
	IO(Memory &mem, serial_kbd &kbd): _mem(mem), _kbd(kbd) {}

	uint8_t in(uint16_t p, processor_t *cpu);
	void out(uint16_t p, uint8_t b, processor_t *cpu);

	void reset();
private:
	serial_kbd &_kbd;
	bool _brk;
	uint8_t kbd_poll();

	void dsk_reset();
	uint8_t dsk_read();
	uint8_t dsk_write();
	bool dsk_seek();
	uint8_t dsk_select(uint8_t a);
	uint8_t dsk_settrk(uint8_t a);
	uint8_t dsk_setsec(uint8_t a);
	uint8_t settrk, setsec, trk, sec;
	uint16_t setdma;
	uint8_t dsk_status;
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
