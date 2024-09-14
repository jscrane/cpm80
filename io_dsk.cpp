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
#include <display.h>
#include <serial_dsp.h>

#include "config.h"
#include "banked_memory.h"
#include "io.h"

static File drive;

#define IMAGE_LEN	20
#define DRIVE_LETTERS	26

typedef struct disk_parameters {
	uint8_t tracks, seclen;
	uint16_t sectrk;
	char image[IMAGE_LEN];
} disk_params_t;

disk_params_t disk_params[DRIVES];
disk_params_t *drive_letters[DRIVE_LETTERS], *dp;

static unsigned read_unsigned(File map) {
	unsigned u = 0;
	for(;;) {
		char ch = map.read();
		if (ch < '0' || ch > '9')
			break;
		u = u * 10 + (ch - '0');
	}
	return u;
}

void IO::dsk_reset() {
	trk = sec = 0xff;
#if defined(USE_SD)
	File map = SD.open(PROGRAMS "drivemap.txt", FILE_READ);
#elif defined(USE_SPIFFS)
	File map = SPIFFS.open(PROGRAMS "drivemap.txt", "r");
#elif defined(USE_LITTLEFS)
	File map = LittleFS.open(PROGRAMS "drivemap.txt", "r");
#endif
	if (!map) {
		DBG(println(F("drivemap: open failed")));
		return;
	}

	for (int i = 0; i < DRIVES; i++) {
		int ch = map.read();
		if (ch == -1)
			break;
		if (ch == '\n')
			continue;
		disk_params_t *p = &disk_params[i];
		drive_letters[ch - 'A'] = p;
		map.read();	// skip ':'
		// read image-name
		for (int j = 0; j < IMAGE_LEN; j++) {
			ch = map.read();
			if (ch == ' ') {
				p->image[j] = 0;
				break;
			}
			p->image[j] = ch;
		}
		p->tracks = read_unsigned(map);
		p->seclen = read_unsigned(map);
		p->sectrk = read_unsigned(map);
		DBG(printf("%s: %d %d %d\r\n", p->image, p->tracks, p->seclen, p->sectrk));
	}
	map.close();

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
		int ok = drive.seek(dp->seclen*(dp->sectrk*trk + sec -1));
		return ok == 1;
	}
	return true;
}

uint8_t IO::dsk_read() {

	if (!dsk_seek())
		return SEEK_ERROR;

	uint8_t buf[dp->seclen];
	int n = drive.read(buf, sizeof(buf));
	if (n < 0)
		return READ_ERROR;

	for (int i = 0; i < n; i++)
		_mem[setdma + i] = buf[i];
	sec++;
	return OK;
}

uint8_t IO::dsk_write() {

	if (!dsk_seek())
		return SEEK_ERROR;

	uint8_t buf[dp->seclen];
	for (unsigned i = 0; i < sizeof(buf); i++)
		buf[i] = _mem[setdma + i];
	int n = drive.write(buf, sizeof(buf));
	if (n < 0)
		return WRITE_ERROR;
	sec++;
	return OK;
}

uint8_t IO::dsk_select(uint8_t a) {

	if (!drive_letters[a]) {
		DBG(printf("dsk_select: %d\r\n", a));
		return ILLEGAL_DRIVE;
	}

	dp = drive_letters[a];

	trk = sec = 0xff;
	if (drive)
		drive.close();
	char buf[32];
	snprintf(buf, sizeof(buf), PROGRAMS"%s", dp->image);
#if defined(USE_SD)
	drive = SD.open(buf, FILE_READ);
#elif defined(USE_SPIFFS)
	drive = SPIFFS.open(buf, "r+");
#elif defined(USE_LITTLEFS)
	drive = LittleFS.open(buf, "r+");
#endif
	if (!drive) {
		DBG(print(buf));
		DBG(println(F(": open failed")));
		return ILLEGAL_DRIVE;
	}

	return OK;
}

// tracks are numbered from 0
uint8_t IO::dsk_settrk(uint8_t a) {

	if (a >= dp->tracks) {
		DBG(printf("settrk: %d\r\n", a));
		return ILLEGAL_TRACK;
	}

	settrk = a;
	return OK;
}

// sectors are numbered from 1
uint8_t IO::dsk_setsec(uint8_t a) {

	if (a > dp->sectrk) {
		DBG(printf("setsec: %d\r\n", a));
		return ILLEGAL_SECTOR;
	}

	setsec = a;
	return OK;
}
