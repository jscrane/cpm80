#ifndef __IO_H__
#define __IO_H__

struct font {
	const uint8_t *data;
	uint8_t w, h, off;
};

class IO: public PortDevice<i8080>, public UTFTDisplay {
public:
	IO(Memory &mem): _mem(mem) {}

	uint8_t in(uint16_t p, i8080 *cpu);
	void out(uint16_t p, uint8_t b, i8080 *cpu);

	void reset();
private:
	void kbd_reset();
	uint8_t kbd_read();
	uint8_t kbd_avail();
	bool kbd_modifier(unsigned scan, bool key_down);
	bool _shift, _ctrl;

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
	void scr_draw(struct font &, char, unsigned, unsigned);
	void scr_display(uint8_t);
	bool _esc, _ansi;
	unsigned _value, _line;
};

#endif
