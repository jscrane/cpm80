#ifndef __IO_H__
#define __IO_H__

struct font {
	const byte *data;
	byte w, h, off;
};

class IO: public i8080::Ports, public UTFTDisplay {
public:
	IO(Memory &);

	byte in(byte p, i8080 *cpu);
	void out(byte p, byte b, i8080 *cpu);

	void reset();
private:
	byte kbd_read();
	void cls();

	void draw(struct font &, char, unsigned, unsigned);
	void display(byte);

	byte settrk, setsec, trk, sec;
	word setdma;
	Memory &_mem;
	bool _shift, _esc, _ansi, _ansi2;
};

#endif
