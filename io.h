#ifndef __IO_H__
#define __IO_H__

struct font {
	const byte *data;
	byte w, h, off;
};

class IO: public i8080::Ports, public UTFTDisplay {
public:
	IO(Memory &mem): _mem(mem) {}

	byte in(byte p, i8080 *cpu);
	void out(byte p, byte b, i8080 *cpu);

	void reset();
private:
	void kbd_reset();
	byte kbd_read();
	byte kbd_avail();
	bool kbd_modifier(unsigned scan, bool key_down);
	bool _shift, _ctrl;

	void dsk_reset();
	byte dsk_read();
	byte dsk_write();
	void dsk_seek();
	void dsk_select(byte a);
	byte settrk, setsec, trk, sec;
	word setdma;
	Memory &_mem;

	void scr_reset();
	void scr_clear();
	void scr_draw(struct font &, char, unsigned, unsigned);
	void scr_display(byte);
	bool _esc, _ansi, _ansi2;
};

#endif
