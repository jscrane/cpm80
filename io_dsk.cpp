#include <Energia.h>
#include <SD.h>
#include <r65emu.h>
#include <i8080.h>

#include "io.h"
#include "config.h"

static File drive;
static char mapping[DRIVES * 13];
static char *drives[DRIVES];

void IO::dsk_reset() {
	trk = sec = 0xff;
	File map = SD.open(PROGRAMS"drivemap.txt", O_READ);
	if (map) {
		int n = map.read(mapping, sizeof(mapping));
		map.close();
		char *p = mapping, *q = p;
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

byte IO::dsk_read() {
	dsk_seek();
	byte buf[128];
	int n = drive.read(buf, sizeof(buf));
	sec++;
	byte c = (n < 0);
	for (int i = 0; i < n; i++)
		_mem[setdma + i] = buf[i];
	return c;
}

byte IO::dsk_write() {
	dsk_seek();
	byte buf[128];
	for (int i = 0; i < sizeof(buf); i++)
		buf[i] = _mem[setdma + i];
	int n = drive.write(buf, sizeof(buf));
	sec++;
	return n < 0;
}

void IO::dsk_select(byte a) {
	trk = sec = 0xff;
	if (drive)
		drive.close();
	char buf[32];
	snprintf(buf, sizeof(buf), PROGRAMS"%s", drives[a]);
	drive = SD.open(buf, O_READ | O_WRITE);
	if (!drive) {
		Serial.print(buf);
		Serial.println(": open failed");
	}
}
