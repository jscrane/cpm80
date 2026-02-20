#pragma once

class screen: public serial_dsp {
public:
	void write(uint8_t);
	void reset();

	void status(const char *s) { display.status(s); }
private:
	Display display;

	void clear();
	void draw(char, unsigned, unsigned);
	bool _esc, _ansi;
	unsigned _value, _line;
};
