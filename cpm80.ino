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

static bool debug_cpu;

static void function_key(uint8_t fn) {
	if (fn == 1)
		machine.reset();
	else if (fn == 10)
		debug_cpu = !debug_cpu;
	else if (fn == 11)
		screen.statusf("%dHz", machine.current_speed());
}

void setup(void) {

	cpu.set_port_out_handler([](uint16_t port, uint8_t b) { io.out(port, b); });
	cpu.set_port_in_handler([](uint16_t port) { return io.in(port); });

	machine.begin();

	io.register_timer_interrupt_handler([]() { cpu.irq(0xff); });
	kbd.register_fnkey_handler(function_key);

	DBG_INI("RAM:	%dkB at 0x%04x", RAM_PAGES, RAM_BASE);
	for (unsigned i = 0; i < RAM_PAGES; i++)
		memory.put(pages[i], RAM_BASE + 1024*i);

#if defined(USE_SPIRAM)
	DBG_INI("SpiRAM: %dkB at 0x%04x", SPIRAM_EXTENT / 1024, SPIRAM_BASE);
	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#endif

	machine.set_cpu_debugging([]() { return debug_cpu; });
	machine.register_reset_handler(reset);
	machine.reset();
}

void loop(void) {

	machine.run();
}
