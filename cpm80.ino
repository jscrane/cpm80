#include <stdarg.h>
#include <SPI.h>

#include <r65emu.h>

#include "config.h"
#include PROCESSOR_H
#include "io.h"

#if defined(USE_HOST_KBD)
hw_serial_kbd kbd(Serial);
#else
ps2_serial_kbd kbd;
#endif

#if defined(USE_HOST_DISPLAY)
hw_serial_dsp screen(Serial);
#else
#include "screen.h"
screen screen;
#endif

#include "banked_memory.h"
BankedMemory memory;

IO io(memory, kbd, screen);
processor_t cpu(memory);
Arduino machine(cpu);

#if defined(BRAM_PAGES)
ram<> boot[BRAM_PAGES];
#endif

#if defined(RAM_PAGES)
ram<> pages[RAM_PAGES];
#endif

static void reset(bool disk) {

	io.reset();
	if (!disk) {
		ERR("Disk initialisation failed");
		screen.status("Disk initialisation failed!");
	}
}

static void function_key(uint8_t fn) {
	if (fn == 1)
		machine.reset();
	else if (fn == 10)
		machine.debug_cpu();
}

void setup(void) {

	cpu.set_port_out_handler([](uint16_t port, uint8_t b) { io.out(port, b); });
	cpu.set_port_in_handler([](uint16_t port) { return io.in(port); });

	machine.begin();

	io.register_timer_interrupt_handler([]() { cpu.irq(0xff); });
	kbd.register_fnkey_handler(function_key);

#if defined(BRAM_PAGES)
	for (unsigned i = 0; i < BRAM_PAGES; i++)
		memory.put(boot[i], BRAM_BASE + 1024*i);
#endif

#if defined(USE_SPIRAM)
	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#endif

	for (unsigned i = 0; i < RAM_PAGES; i++)
		memory.put(pages[i], RAM_BASE + 1024*i);

	machine.register_reset_handler(reset);
	machine.reset();
}

void loop(void) {

	machine.run(10);
}
