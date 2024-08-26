#include <Arduino.h>
#include <hardware.h>

#if defined(USE_SD)
#include <SD.h>
#elif defined(USE_SPIFFS)
#include <FS.h>
#include <SPIFFS.h>
#elif defined(USE_LITTLEFS)
#include <FS.h>
#include <LittleFS.h>
#endif

#include <memory.h>
#include <CPU.h>
#include <ports.h>
#include <i8080.h>
#include <display.h>

#include "io.h"
#include "config.h"

static File drive;
static uint8_t mapping[DRIVES * 13];
static uint8_t *drives[DRIVES];

void IO::dsk_reset() {
	trk = sec = 0xff;
#if defined(USE_SD)
	File map = SD.open(PROGRAMS "drivemap.txt", O_READ);
#elif defined(USE_SPIFFS)
	File map = SPIFFS.open(PROGRAMS "drivemap.txt", "r");
#elif defined(USE_LITTLEFS)
	File map = LittleFS.open(PROGRAMS "drivemap.txt", "r");
#endif
	if (!map) {
		DBG(println(F("drivemap: open failed")));
		return;
	}

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

	// read boot sector
	settrk = 0;
	setsec = 1;
	setdma = 0;
	dsk_select(0);
	dsk_seek();
	dsk_read();
}

bool IO::dsk_seek() {
	if (trk != settrk || sec != setsec) {
		trk = settrk;
		sec = setsec;
		int ok = drive.seek(SECLEN*(SECTRK*trk + sec -1));
		return ok == 1;
	}
	return true;
}

uint8_t IO::dsk_read() {

	dsk_led(RED);

	if (!dsk_seek()) {
		dsk_led();
		return SEEK_ERROR;
	}

	uint8_t buf[SECLEN];
	int n = drive.read(buf, sizeof(buf));
	if (n < 0) {
		dsk_led();
		return READ_ERROR;
	}

	for (int i = 0; i < n; i++)
		_mem[setdma + i] = buf[i];
	sec++;
	dsk_led();
	return OK;
}

uint8_t IO::dsk_write() {

	dsk_led(BLUE);

	if (!dsk_seek()) {
		dsk_led();
		return SEEK_ERROR;
	}

	uint8_t buf[SECLEN];
	for (unsigned i = 0; i < sizeof(buf); i++)
		buf[i] = _mem[setdma + i];
	int n = drive.write(buf, sizeof(buf));
	if (n < 0) {
		dsk_led();
		return WRITE_ERROR;
	}
	sec++;
	dsk_led();
	return OK;
}

uint8_t IO::dsk_select(uint8_t a) {

	if (a >= DRIVES) {
		DBG(printf("dsk_select: %d\r\n", a));
		return ILLEGAL_DRIVE;
	}

	dsk_led(RED);
	trk = sec = 0xff;
	if (drive)
		drive.close();
	char buf[32];
	snprintf(buf, sizeof(buf), PROGRAMS"%s", drives[a]);
#if defined(USE_SD)
	drive = SD.open(buf, O_READ | O_WRITE);
#elif defined(USE_SPIFFS)
	drive = SPIFFS.open(buf, "r+");
#elif defined(USE_LITTLEFS)
	drive = LittleFS.open(buf, "r+");
#endif
	dsk_led();

	if (!drive) {
		DBG(print(buf));
		DBG(println(F(": open failed")));
		return ILLEGAL_DRIVE;
	}

	return OK;
}

uint8_t IO::dsk_settrk(uint8_t a) {

	if (a > TRACKS) {
		DBG(printf("settrk: %d\r\n", a));
		return ILLEGAL_TRACK;
	}

	settrk = a;
	return OK;
}

uint8_t IO::dsk_setsec(uint8_t a) {

	if (a > SECTRK) {
		DBG(printf("setsec: %d\r\n", a));
		return ILLEGAL_SECTOR;
	}

	setsec = a;
	return OK;
}
