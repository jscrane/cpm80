#include <stdarg.h>
#include <UTFT.h>
#include <SPI.h>
#include <SpiRAM.h>
#include <SD.h>
#include <r65emu.h>
#include <i8080.h>

#include "config.h"
#include "io.h"
#include "roms/cpm22.h"
#include "roms/cbios.h"

void status(const char *fmt, ...) {
	char tmp[256];  
	va_list args;
	va_start(args, fmt);
	vsnprintf(tmp, sizeof(tmp), fmt, args);
	Serial.println(tmp);
	va_end(args);
}

IO io(memory);
jmp_buf ex;
i8080 cpu(memory, ex, status, io);
bool halted = false;
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

	halted = (setjmp(ex) != 0);
}

void setup(void) {
	Serial.begin(115200);
	hardware_init(cpu);

	memory.put(sram, 0x0000);

	for (unsigned i = 0; i < 7; i++)
		memory.put(pages[i], 0xe400 + 1024*i);

	reset();
}

void loop(void) {
	if (!halted)
		cpu.run(INSTRUCTIONS);
}
