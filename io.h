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

	void start(const char *dir);

private:
	byte kbd_read();
	void reset();
	void draw(struct font &, char, unsigned, unsigned);
	void display(byte);

	byte seldsk, settrk, setsec, trk, sec;
	word setdma;
	Memory &_mem;
	bool _shift;
};

#endif
