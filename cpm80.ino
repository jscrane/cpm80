#include <stdarg.h>
#include <UTFT.h>
#include <SPI.h>
#include <SpiRAM.h>
#include <SD.h>
#include <r65emu.h>
#include <ports.h>
#include <i8080.h>

#include "config.h"
#include "io.h"
#include "roms/cpm22.h"
#include "roms/cbios.h"

IO io(memory);
i8080 cpu(memory, io);
ram pages[7];

void reset(void) {
	bool sd = hardware_reset();

	unsigned i;
	for (i = 0; i < sizeof(cpm22); i++)
		memory[0xe400 + i] = cpm22[i];
	for (i = 0; i < sizeof(cbios); i++)
		memory[0xfa00 + i] = cbios[i];

	if (sd)
		io.reset();
	else
		Serial.println("No SD Card");

	// set up jump to bios
	memory[0] = 0xc3;
	memory[1] = 0x00;
	memory[2] = 0xfa;
	cpu.reset();
//	cpu.debug();
}

void setup(void) {
	Serial.begin(115200);
	hardware_init(cpu);

	memory.put(sram, 0x0000, 0xe4);

	for (unsigned i = 0; i < 7; i++)
		memory.put(pages[i], 0xe400 + 1024*i);

	reset();
}

void loop(void) {
	if (!cpu.halted())
		cpu.run(INSTRUCTIONS);
}
