#include <stdint.h>

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
#include <display.h>
#include <serial_dsp.h>
#include <machine.h>
#include <debugging.h>
#include <arduinomachine.h>

#include "config.h"
#include "banked_memory.h"
#include "io.h"
#include "disk.h"

#define IMAGE_LEN	20
#define DRIVE_LETTERS	26

#define MODE_READ	"r"
#define MODE_READWRITE	"r+"
#define MODE_WRITE	"w"

static File drive;

typedef struct disk_parameters {
	uint8_t tracks, seclen;
	uint16_t sectrk;
	char image[IMAGE_LEN];
} disk_params_t;

static disk_params_t disk_params[DRIVES];
static disk_params_t *drive_letters[DRIVE_LETTERS], *dp;

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

void Disk::reset() {
	_trk = _sec = 0xff;

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
		DBG_DISK("%s: %d %d %d", p->image, p->tracks, p->seclen, p->sectrk);
	}
	map.close();
}

bool Disk::seek() {

	if (_trk != _settrk || _sec != _setsec) {
		_trk = _settrk;
		_sec = _setsec;
		int ok = drive.seek(dp->seclen*(dp->sectrk*_trk + _sec -1));
		return ok == 1;
	}
	return true;
}

uint8_t Disk::read(Memory &mem) {

	if (!seek()) {
		ERR("dsk_read: seek error");
		return status(SEEK_ERROR);
	}

	uint8_t buf[dp->seclen];
	int n = drive.read(buf, sizeof(buf));
	if (n < 0 || n != sizeof(buf)) {
		ERR("dsk_read: read error");
		return status(READ_ERROR);
	}
	for (int i = 0; i < n; i++)
		mem[_setdma + i] = buf[i];
	_sec++;
	return status(OK);
}

uint8_t Disk::write(Memory &mem) {

	if (!seek()) {
		ERR("dsk_write: seek error");
		return status(SEEK_ERROR);
	}

	uint8_t buf[dp->seclen];
	for (unsigned i = 0; i < sizeof(buf); i++)
		buf[i] = mem[_setdma + i];

	int n = drive.write(buf, sizeof(buf));
	if (n < 0 || n != sizeof(buf)) {
		ERR("dsk_write: write error");
		return status(WRITE_ERROR);
	}
	_sec++;
	return status(OK);
}

uint8_t Disk::select(uint8_t a) {

	if (!drive_letters[a]) {
		ERR("dsk_select: %d", a);
		return status(ILLEGAL_DRIVE);
	}

	if (dp == drive_letters[a])
		return status(OK);

	dp = drive_letters[a];
	_trk = _sec = 0xff;
	if (drive)
		drive.close();

	char buf[64];
	snprintf(buf, sizeof(buf), PROGRAMS "%s", dp->image);
	drive = DISK.open(buf, MODE_READWRITE);
	if (!drive) {
		ERR("%s: open failed", buf);
		return status(ILLEGAL_DRIVE);
	}

	return status(OK);
}

// tracks are numbered from 0
uint8_t Disk::track(uint8_t a) {

	if (a >= dp->tracks) {
		ERR("settrk: %d", a);
		return status(ILLEGAL_TRACK);
	}

	_settrk = a;
	return status(OK);
}

// sectors are numbered from 1
uint8_t Disk::sector(uint16_t a) {

	if (a > dp->sectrk) {
		ERR("setsec: %d", a);
		return status(ILLEGAL_SECTOR);
	}

	_setsec = a;
	return status(OK);
}

bool IO::checkpoint() {

	File img = DISK.open(PROGRAMS "machine.img", MODE_WRITE);
	if (!img)
		return false;

	StreamCheckpoint c(img);
	_machine->checkpoint(c);
	img.close();
	return true;
}

bool IO::restore() {

	File img = DISK.open(PROGRAMS "machine.img", MODE_READ);
	if (!img)
		return false;

	StreamCheckpoint c(img);
	_machine->checkpoint(c);
	img.close();
	return true;
}
