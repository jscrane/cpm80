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
#include "flash_disk.h"

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

void FlashDisk::reset() {

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

uint8_t FlashDisk::select(uint8_t a) {

	if (!drive_letters[a]) {
		ERR("dsk_select: %d", a);
		return status(ILLEGAL_DRIVE);
	}

	if (dp == drive_letters[a])
		return status(OK);

	dp = drive_letters[a];
	_drive_changed();
	if (drive)
		drive.close();

	char buf[64];
	snprintf(buf, sizeof(buf), PROGRAMS "%s", dp->image);
	drive = DISK.open(buf, MODE_READWRITE);
	if (!drive) {
		ERR("%s: open failed", buf);
		return status(ILLEGAL_DRIVE);
	}

	_tracks = dp->tracks;
	_seclen = dp->seclen;
	_sectrk = dp->sectrk;

	return status(OK);
}

bool FlashDisk::_seek(long pos) {
	return drive.seek(pos) == 1;
}

int FlashDisk::_read(uint8_t *buf, size_t len) {
	return drive.read(buf, len);
}

int FlashDisk::_write(const uint8_t *buf, size_t len) {
	return drive.write(buf, len);
}

bool FlashDisk::checkpoint() {

	File img = DISK.open(PROGRAMS "machine.img", MODE_WRITE);
	if (!img)
		return false;

	StreamCheckpoint c(img);
	_machine->checkpoint(c);
	img.close();
	return true;
}

bool FlashDisk::restore() {

	File img = DISK.open(PROGRAMS "machine.img", MODE_READ);
	if (!img)
		return false;

	StreamCheckpoint c(img);
	_machine->restore(c);
	img.close();
	return true;
}
