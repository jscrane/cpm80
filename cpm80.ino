#include <stdarg.h>
#include <r65emu.h>

#include <SPI.h>
#if defined(USE_UTFT)
//#include <UTFT.h>
#elif defined(USE_ESPI)
#include <TFT_eSPI.h>
#endif

#if defined(USE_SD)
#include <SD.h>
#elif defined(USE_SPIFFS)
#include <FS.h>
#include <SPIFFS.h>
#elif defined(ESP8266)
#include <FS.h>
#endif

#if defined(SPIRAM_CS)
#include <SpiRAM.h>
#endif

#include <ports.h>
#include <i8080.h>

#include "config.h"
#include "io.h"
#include "roms/cpm22.h"
#include "roms/cbios.h"

IO io(memory);
i8080 cpu(memory, io);

ram boot[BRAM_PAGES];

#if defined(RAM_PAGES)
ram pages[RAM_PAGES];
#endif

void reset(void) {
	bool sd = hardware_reset();

	unsigned i;
	for (i = 0; i < sizeof(cpm22); i++)
		memory[BRAM_BASE + i] = pgm_read_byte(&cpm22[i]);
	for (i = 0; i < sizeof(cbios); i++)
		memory[0xfa00 + i] = pgm_read_byte(&cbios[i]);

	if (sd)
		io.reset();
	else
		Serial.println("No SD Card");

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

	for (unsigned i = 0; i < BRAM_PAGES; i++)
		memory.put(boot[i], BRAM_BASE + 1024*i);

#if defined(SPIRAM_CS)
	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#else
	for (unsigned i = 0; i < RAM_PAGES; i++)
		memory.put(pages[i], RAM_BASE + 1024*i);
#endif
	reset();
}

void loop(void) {
	if (!cpu.halted())
		cpu.run(INSTRUCTIONS);
	yield();
}
