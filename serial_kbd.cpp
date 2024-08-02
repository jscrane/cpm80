#include <Arduino.h>

#include "config.h"

#if defined(SERIAL_KBD)
#include "kbd.h"
#include "serial_kbd.h"

uint8_t serialkbd::read() {
	while (!_serial.available()) {
#if !defined(Energia_h)
		yield();
#endif
	}

	return _serial.read();
}

uint8_t serialkbd::avail() {
	return _serial.available()? 0xff: 0x00;
}

void serialkbd::reset() {
	_serial.begin(TERMINAL_SPEED);
}
#endif
