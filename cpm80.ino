#include <stdarg.h>
#include <SPI.h>

#include <r65emu.h>
#include <ports.h>
#include <i8080.h>

#include "config.h"
#include "io.h"
#include "roms/cpm22.h"
#include "roms/cbios.h"

IO io(memory);
i8080 cpu(memory, io);

#if defined(BRAM_PAGES)
ram boot[BRAM_PAGES];
#endif

#if defined(RAM_PAGES)
ram pages[RAM_PAGES];
#endif

void reset(void) {
	bool disk = hardware_reset();

	unsigned i;
	for (i = 0; i < sizeof(cpm22); i++)
		memory[BRAM_BASE + i] = pgm_read_byte(&cpm22[i]);
	for (i = 0; i < sizeof(cbios); i++)
		memory[0xfa00 + i] = pgm_read_byte(&cbios[i]);

	if (disk)
		io.reset();
	else
		Serial.println("Disk initialisation failed");

	// set up jump to bios
	memory[0] = 0xc3;
	memory[1] = 0x00;
	memory[2] = 0xfa;
	cpu.reset();
}

void setup(void) {
#if defined(DEBUGGING)
	Serial.begin(115200);
#endif
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

	reset();
}

void loop(void) {
	if (!cpu.halted())
		cpu.run(INSTRUCTIONS);
}
