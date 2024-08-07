#include <Arduino.h>
#include "config.h"

#if defined(PS2_ADV_KBD)
#include <PS2KeyAdvanced.h>
#include <PS2KeyMap.h>
#include <hardware.h>
#include "kbd.h"
#include "ps2_adv_kbd.h"

PS2KeyAdvanced keyboard;
PS2KeyMap keymap;

void ps2advkbd::reset() {
	keyboard.begin(PS2_KBD_DATA, PS2_KBD_IRQ);
	keyboard.setNoBreak(1);
	keymap.selectMap(PS2_ADV_KBD_LAYOUT);
}

uint8_t ps2advkbd::avail() {
	return keyboard.available()? 0xff: 0x00;
}

uint8_t ps2advkbd::read() {
	for (;;) {
		while (!keyboard.available()) {
#if !defined(Energia_h)
			yield();
#endif
		}
		uint16_t key = keyboard.read();
		uint16_t code = keymap.remapKey(key);
		if (code > 0)
			return code & 0xff;
	}
}
#endif
