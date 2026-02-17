#pragma once

class screen: public Display, public serial_dsp {
public:
	void write(uint8_t);
	void reset();

private:
	void clear();
	void draw(char, unsigned, unsigned);
	bool _esc, _ansi;
	unsigned _value, _line;
};
