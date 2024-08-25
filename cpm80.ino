#include <stdarg.h>
#include <SPI.h>

#include <r65emu.h>
#include <ports.h>
#include <i8080.h>

#include "config.h"
#include "io.h"

#if defined(PS2_SERIAL_KBD)
ps2_serial_kbd kbd;

#elif defined(HW_SERIAL_KBD)
hw_serial_kbd kbd(Serial);

#else
#error "No keyboard defined!"
#endif

IO io(memory, kbd);
i8080 cpu(memory, io);

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
		DBG(println(F("Disk initialisation failed")));

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

	hardware_run();
}
