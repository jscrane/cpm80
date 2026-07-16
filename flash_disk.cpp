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

#define MODE_READ	"r"
#define MODE_READWRITE	"r+"
#define MODE_WRITE	"w"

static File drive;
static char images[DRIVES][IMAGE_LEN];

static uint32_t size(const char *filename) {
	char buf[64];
	snprintf(buf, sizeof(buf), PROGRAMS "%s", filename);
	File file = DISK.open(buf, MODE_READ);
	if (file) {
		uint32_t s = file.size();
		file.close();
		return s;
	}
	ERR("file not found: %s", filename);
	return 0;
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
		int drive_letter = ch - 'A';
		map.read();	// skip ':'
		// read image-name
		char image[IMAGE_LEN];
		for (int j = 0; j < IMAGE_LEN; j++) {
			ch = map.read();
			if (ch == '\n') {
				image[j] = 0;
				break;
			}
			image[j] = ch;
		}
		int drive_id = _add_drive(drive_letter, size(image));
		if (0 > drive_id) {
			ERR("failed to add drive %s", image);
			break;
		}
		strncpy(images[drive_id], image, IMAGE_LEN);
		DBG_DISK("added: %d %s %d", drive_letter, image, drive_id);
	}
	map.close();
}

bool FlashDisk::_open(uint8_t drive_id) {

	if (drive)
		drive.close();

	char buf[64];
	snprintf(buf, sizeof(buf), PROGRAMS "%s", images[drive_id]);
	drive = DISK.open(buf, MODE_READWRITE);
	if (!drive) {
		ERR("%s: open failed", buf);
		return false;
	}
	return true;
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
