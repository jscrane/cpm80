#include <Arduino.h>
#include <UTFT.h>
#include <r65emu.h>
#include <ports.h>
#include <i8080.h>
#include <utftdisplay.h>

#include "io.h"
#include "config.h"

#include "plain_font.h"
//#include "tama_mini02_font.h"

#define ROWS    30
#define COLS    64
//#define COLS    80

static struct font f = { plain_font, 5, 8, 0x20 };
//struct font f = { tama_font, 4, 8, 0x20 };

static unsigned r, c;
static char screen[ROWS][COLS];

void IO::scr_clear() {
	r = c = 0;
	for (int j = 0; j < ROWS; j++)
		for (int i = 0; i < COLS; i++)
			screen[j][i] = ' ';
	clear();
}

void IO::scr_reset() {
	_esc = _ansi = false;
	_line = _value = 0;

	UTFTDisplay::begin(TFT_BG, TFT_FG);
	scr_clear();
}

void IO::scr_draw(struct font &f, char ch, unsigned i, unsigned j) {
	if (screen[j][i] != ch) {
		const uint8_t *p = f.data + f.w * (ch - f.off);
		const uint8_t *q = f.data + f.w * (screen[j][i] - f.off);
		unsigned x = i * f.w;
		for (unsigned c = 0; c < f.w; c++) {
			uint8_t col = *p++, ecol = *q++, d = (col ^ ecol);
			unsigned y = (j + 1)*f.h;
			for (unsigned r = 0, b = 0x80; r < f.h; r++, b /= 2) {
				y--;
				if (d & b) {
					utft.setColor((col & b)? _fg: _bg);
					utft.drawPixel(x, y);
				}
			}
			x++;
		}
		screen[j][i] = ch;
	}
}

void IO::dsk_led(unsigned colour) {
	utft.setColor(colour);
	utft.drawPixel(_dx-1, 0);
}

void IO::scr_display(uint8_t b) {
	char ch = (char)b;
//Serial.println((uint8_t)ch);
	switch(ch) {
	case 0x08:		// '\b'
		scr_draw(f, ' ', c, r);
		if (c-- == 0) {
			r--;
			c = COLS-1;
		}
		break;
	case 0x0d:		// '\r'
		scr_draw(f, ' ', c, r);
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
				for (unsigned i = c; i < COLS; i++)
					scr_draw(f, ' ', i, r);
				break;

			case 'J':
				if (_value == 2)
					scr_clear();
				break;

			case 'A':
				scr_draw(f, ' ', c, r);
				r -= _value;
				break;

			case 'B':
				scr_draw(f, ' ', c, r);
				r += _value;
				break;

			case 'C':
				scr_draw(f, ' ', c, r);
				c += _value;
				break;

			case 'D':
				scr_draw(f, ' ', c, r);
				c -= _value;
				break;

			case 'H':
				scr_draw(f, ' ', c, r);
				r = _line;
				c = _value;
				break;

			default:
				// ???
				Serial.println(ch);
				break;
			}

			if (r < 0) r = 0;
			else if (r > ROWS-1) r = ROWS-1;

			if (c < 0) c = 0;
			if (c > COLS-1) c = COLS-1;

			_value = _line = 0;
			_ansi = false;
		} else {
			_esc = _ansi = false;
			_line = _value = 0;
			if (ch >= 0x20 && ch < 0x7f) {
				scr_draw(f, ch, c, r);
				if (++c == COLS) {
					c = 0;
					r++;
				}
			}
		}
	}
	if (r == ROWS) {
		// scroll
		r--;
		for (unsigned j = 0; j < (ROWS-1); j++)
			for (int i = 0; i < COLS; i++)
				scr_draw(f, screen[j+1][i], i, j);
		for (unsigned i = 0; i < COLS; i++)
			scr_draw(f, ' ', i, ROWS-1);
	}
	scr_draw(f, '_', c, r);
}
