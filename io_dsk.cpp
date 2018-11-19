#include <stdint.h>
#include <hardware.h>

#if defined(USE_SD)
#include <SD.h>
#elif defined(USE_SPIFFS)
#include <FS.h>
#include <SPIFFS.h>
#elif defined(USE_FS)
#include <FS.h>
#endif

#include <sdtape.h>
#include <checkpoint.h>
#include <memory.h>
#include <CPU.h>
#include <ports.h>
#include <i8080.h>
#include <tftdisplay.h>

#include "io.h"
#include "config.h"

static File drive;
static uint8_t mapping[DRIVES * 13];
static uint8_t *drives[DRIVES];

void IO::dsk_reset() {
	trk = sec = 0xff;
#if defined(USE_SD)
	File map = SD.open(PROGRAMS"drivemap.txt", O_READ);
#elif defined(USE_SPIFFS) || defined(USE_FS)
	File map = SPIFFS.open(PROGRAMS"drivemap.txt", "r");
#endif
	if (map) {
		int n = map.read(mapping, sizeof(mapping));
		map.close();
		uint8_t *p = mapping, *q = p;
		for (unsigned i = 0; i < DRIVES; i++) {
			while (*p != '\n')
				p++;
			*p++ = 0;
			drives[i] = q;
			q = p;
			if (p - mapping >= n)
				break;
		}
	} else
		Serial.println("drivemap: open failed");
}

void IO::dsk_seek() {
	if (trk != settrk || sec != setsec) {
		trk = settrk;
		sec = setsec;
		drive.seek(128*(26*trk + sec -1));
	}
}

uint8_t IO::dsk_read() {
	dsk_led(RED);
	dsk_seek();
	uint8_t buf[128];
	int n = drive.read(buf, sizeof(buf));
	sec++;
	uint8_t c = (n < 0);
	for (int i = 0; i < n; i++)
		_mem[setdma + i] = buf[i];
	dsk_led();
	return c;
}

uint8_t IO::dsk_write() {
	dsk_led(BLUE);
	dsk_seek();
	uint8_t buf[128];
	for (unsigned i = 0; i < sizeof(buf); i++)
		buf[i] = _mem[setdma + i];
	int n = drive.write(buf, sizeof(buf));
	sec++;
	dsk_led();
	return n < 0;
}

void IO::dsk_select(uint8_t a) {
	dsk_led(RED);
	trk = sec = 0xff;
	if (drive)
		drive.close();
	char buf[32];
	snprintf(buf, sizeof(buf), PROGRAMS"%s", drives[a]);
#if defined(USE_SD)
	drive = SD.open(buf, O_READ | O_WRITE);
#elif defined(USE_SPIFFS) || defined(USE_FS)
	drive = SPIFFS.open(buf, "r+");
#endif
	if (drive) 
		dsk_led();
	else {
		Serial.print(buf);
		Serial.println(": open failed");
	}
}
