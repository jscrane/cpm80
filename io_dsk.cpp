#include <Arduino.h>
#include <machine.h>
#include <hardware.h>

#if defined(USE_SD)
#include <SD.h>
#define DISK SD

#elif defined(USE_SPIFFS)
#include <FS.h>
#include <SPIFFS.h>
#define DISK SPIFFS

#elif defined(USE_LITTLEFS)
#include <FS.h>
#include <LittleFS.h>
#define DISK LittleFS

#else
#error "This application requires storage. (Did you configure hw/user.h?)"
#endif

#include <memory.h>
#include <CPU.h>
#include <display.h>
#include <serial_dsp.h>
#include <debugging.h>

#include "config.h"
#include "banked_memory.h"
#include "io.h"

static File drive;

#define IMAGE_LEN	20
#define DRIVE_LETTERS	26

#define MODE_READ	"r"
#if defined(USE_LITTLEFS) && defined(LITTLEFS_READ_MODE)
#define MODE_READWRITE	LITTLEFS_READ_MODE
#else
#define MODE_READWRITE	"r+"
#endif

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

	File map = DISK.open(PROGRAMS "drivemap.txt", MODE_READ);
	if (!map) {
		ERR(PROGRAMS "drivemap.txt: open failed");
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
		DBG_DISK("%s: %d %d %d\r\n", p->image, p->tracks, p->seclen, p->sectrk);
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
		ERR("dsk_select: %d", a);
		return ILLEGAL_DRIVE;
	}

	if (dp == drive_letters[a])
		return OK;

	dp = drive_letters[a];

	trk = sec = 0xff;
	if (drive)
		drive.close();

	char buf[32];
	snprintf(buf, sizeof(buf), PROGRAMS "%s", dp->image);
	drive = DISK.open(buf, MODE_READWRITE);
	if (!drive) {
		ERR("%s: open failed", buf);
		return ILLEGAL_DRIVE;
	}

	return OK;
}

// tracks are numbered from 0
uint8_t IO::dsk_settrk(uint8_t a) {

	if (a >= dp->tracks) {
		ERR("settrk: %d\r\n", a);
		return ILLEGAL_TRACK;
	}

	settrk = a;
	return OK;
}

// sectors are numbered from 1
uint8_t IO::dsk_setsec(uint16_t a) {

	if (a > dp->sectrk) {
		ERR("setsec: %d\r\n", a);
		return ILLEGAL_SECTOR;
	}

	setsec = a;
	return OK;
}
