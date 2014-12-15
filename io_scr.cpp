#include <Energia.h>
#include <UTFT.h>
#include <r65emu.h>
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
	_esc = _ansi = _ansi2 = false;

	UTFTDisplay::begin(TFT_BG, TFT_FG);
	scr_clear();
}

void IO::scr_draw(struct font &f, char ch, unsigned i, unsigned j) {
	if (screen[j][i] != ch) {
		const byte *p = f.data + f.w * (ch - f.off);
		const byte *q = f.data + f.w * (screen[j][i] - f.off);
		unsigned x = i * f.w;
		for (unsigned c = 0; c < f.w; c++) {
			byte col = *p++, ecol = *q++, d = (col ^ ecol);
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

void IO::scr_display(byte b) {
	char ch = (char)b;
Serial.println((byte)ch);
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
		if (_esc && ch == '[') {
			_ansi = true;
			_esc = false;
			return;
		}
		if (_ansi && ch == '2') {
			_ansi2 = true;
			_ansi = false;
			return;
		}
		if (_ansi2 && ch == 'J') {
			scr_clear();
			_ansi2 = false;
			return;
		}
		_esc = _ansi = _ansi2 = false;
		if (ch >= 0x20 && ch < 0x7f) {
			scr_draw(f, ch, c, r);
			if (++c == COLS) {
				c = 0;
				r++;
			}
		}
	}
	if (r == ROWS) {
		// scroll
		r--;
		for (int j = 0; j < (ROWS-1); j++)
			for (int i = 0; i < COLS; i++)
				scr_draw(f, screen[j+1][i], i, j);
		for (int i = 0; i < COLS; i++)
			scr_draw(f, ' ', i, ROWS-1);
	}
	scr_draw(f, '_', c, r);
}
