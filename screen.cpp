#include <Arduino.h>
#include <functional>

#include <machine.h>
#include <memory.h>
#include <CPU.h>
#include <display.h>
#include <serial_dsp.h>
#include <hardware.h>
#include <debugging.h>

#if !defined(USE_HOST_DISPLAY)

#include "config.h"
#include "screen.h"
#include "io.h"

#define ROWS	24
#define COLS	80

static unsigned r, c;
static unsigned rows, cols;
static char buf[ROWS][COLS];

void screen::clear() {
	r = c = 0;
	for (unsigned j = 0; j < ROWS; j++)
		for (unsigned i = 0; i < COLS; i++)
			buf[j][i] = ' ';
	display.clear();
}

void screen::reset() {
	_esc = _ansi = false;
	_line = _value = 0;

	display.begin(BG_COLOUR, FG_COLOUR, ORIENT);
	rows = display.height() / display.charHeight();
	if (rows > ROWS) rows = ROWS;
	cols = display.width() / display.charWidth();
	if (cols > COLS) cols = COLS;
	DBG_DSP("screen %ux%u", cols, rows);
	display.setScreenChars(cols, rows);
	clear();
}

void screen::draw(char ch, unsigned i, unsigned j) {
	if (buf[j][i] != ch) {
		display.setCursor(i * display.charWidth(), j * display.charHeight());
		display.print(ch);
		buf[j][i] = ch;
	}
}

void screen::write(uint8_t b) {
	char ch = (char)b;
	switch(ch) {
	case 0x08:		// '\b'
		draw(' ', c, r);
		if (c-- == 0) {
			r--;
			c = cols-1;
		}
		break;
	case 0x0d:		// '\r'
		draw(' ', c, r);
		c = 0;
		return;		// no prompt
	case 0x0a:		// '\n'
		r++;
		break;
	case 0x1b:		// esc
		_esc = true;
		return;
	default:
		if (_esc) {
			_esc = false;
			if (ch == '[') {
				_ansi = true;
				return;
			}
		}
		if (_ansi) {
			switch (ch) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				_value = _value * 10 + ch - '0';
				return;

			case ';':
				_line = _value;
				_value = 0;
				return;

			case 'K':
				for (unsigned i = c; i < cols; i++)
					draw(' ', i, r);
				break;

			case 'J':
				if (_value == 2)
					clear();
				break;

			case 'A':
				draw(' ', c, r);
				r -= _value;
				break;

			case 'B':
				draw(' ', c, r);
				r += _value;
				break;

			case 'C':
				draw(' ', c, r);
				c += _value;
				break;

			case 'D':
				draw(' ', c, r);
				c -= _value;
				break;

			case 'H':
				draw(' ', c, r);
				r = _line;
				c = _value;
				break;

			default:
				// ???
				DBG_DSP("%x", ch);
				break;
			}

			if (r < 0) r = 0;
			else if (r > rows-1) r = rows-1;

			if (c < 0) c = 0;
			if (c > cols-1) c = cols-1;

			_value = _line = 0;
			_ansi = false;
		} else {
			_esc = _ansi = false;
			_line = _value = 0;
			if (ch >= 0x20 && ch < 0x7f) {
				draw(ch, c, r);
				if (++c == cols) {
					c = 0;
					r++;
				}
			}
		}
	}
	if (r == rows) {
		// scroll
		r--;
		for (unsigned j = 0; j < (rows-1); j++)
			for (unsigned i = 0; i < cols; i++)
				draw(buf[j+1][i], i, j);
		for (unsigned i = 0; i < cols; i++)
			draw(' ', i, r);
	}
	draw('_', c, r);
}
#endif
