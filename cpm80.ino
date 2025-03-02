#include <stdarg.h>
#include <SPI.h>

#include <r65emu.h>
#include <ports.h>

#include "config.h"
#include PROCESSOR_H
#include "screen.h"
#include "io.h"

#if defined(PS2_SERIAL_KBD)
ps2_serial_kbd kbd;
#else
hw_serial_kbd kbd(Serial);
#endif

#if defined(SCREEN_SERIAL_DSP)
screen dsp;
#else
hw_serial_dsp dsp(Serial);
#endif

#include "banked_memory.h"
BankedMemory memory;

IO io(memory, kbd, dsp);
processor_t cpu(memory, io);

#if defined(BRAM_PAGES)
ram<> boot[BRAM_PAGES];
#endif

#if defined(RAM_PAGES)
ram<> pages[RAM_PAGES];
#endif

void reset(void) {
	bool disk = hardware_reset();

	if (disk)
		io.reset();
	else
		ERR(println(F("Disk initialisation failed")));

	cpu.reset();
}

void function_key(uint8_t fn) {
	if (fn == 1)
		reset();
	else if (fn == 10)
		hardware_debug_cpu();
}

void setup(void) {
	hardware_init(cpu);

#if defined(BRAM_PAGES)
	for (unsigned i = 0; i < BRAM_PAGES; i++)
		memory.put(boot[i], BRAM_BASE + 1024*i);
#endif

#if defined(USE_SPIRAM)
	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#endif

	for (unsigned i = 0; i < RAM_PAGES; i++)
		memory.put(pages[i], RAM_BASE + 1024*i);

	kbd.register_fnkey_handler(function_key);

	reset();
}

void loop(void) {

	if (!hardware_run()) {
		ERR(printf("CPU halted at %04x\r\n", cpu.pc()));
		for(;;) yield();
	}
}
