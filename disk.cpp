#include <stdint.h>
#include <stddef.h>

#include <memory.h>
#include <machine.h>
#include <debugging.h>

#include "disk.h"

typedef struct disk_parameters {
	uint8_t drive_id;
	uint8_t drive_letter;
	uint8_t tracks, seclen;
	uint16_t sectrk;
} disk_params_t;

static disk_params_t disk_params[DRIVES];
static disk_params_t *drive_letters[DRIVE_LETTERS], *dp;
static int next_drive = 0;

static const struct { uint32_t size; uint8_t tracks, seclen; uint16_t sectrk; } known_geometries[] = {
	{ 256256,   77, 128,  26 },	// CP/M 2.2 8" SSSD
	{ 4177920, 255, 128, 128 },	// FUZIX hd-fuzix.dsk
	// more here...
};

static size_t num_geometries = sizeof(known_geometries) / sizeof(known_geometries[0]);

int Disk::_add_drive(int drive_letter, uint32_t size) {

	if (next_drive >= DRIVES || drive_letter >= DRIVE_LETTERS || size == 0)
		return -1;

	disk_params_t *d = &disk_params[next_drive];
	size_t g;
	for (g = 0; g < num_geometries; g++)
		if (size == known_geometries[g].size) {
			d->tracks = known_geometries[g].tracks;
			d->seclen = known_geometries[g].seclen;
			d->sectrk = known_geometries[g].sectrk;
			break;
		}
	if (g == num_geometries) {
		ERR("unknown disk length: %d", size);
		return -1;
	}

	drive_letters[drive_letter] = d;
	d->drive_letter = drive_letter;
	d->drive_id = next_drive++;
	return d->drive_id;
}

uint8_t Disk::select(uint8_t a) {

	if (!drive_letters[a]) {
		ERR("disk select: %d", a);
		return status(ILLEGAL_DRIVE);
	}

	if (dp && dp == drive_letters[a])
		return status(OK);

	if (!_open(drive_letters[a]->drive_id))
		return status(ILLEGAL_DRIVE);

	dp = drive_letters[a];
	_trk = _sec = 0xff;
	return status(OK);
}

bool Disk::seek() {

	if (_trk != _settrk || _sec != _setsec) {
		_trk = _settrk;
		_sec = _setsec;
		return _seek((long)dp->seclen * (dp->sectrk * _trk + _sec - 1));
	}
	return true;
}

uint8_t Disk::read(Memory &mem) {

	if (!seek()) {
		ERR("disk: seek error");
		return status(SEEK_ERROR);
	}

	uint8_t buf[dp->seclen];
	int n = _read(buf, sizeof(buf));
	if (n < 0 || (unsigned)n != sizeof(buf)) {
		ERR("disk: read error");
		return status(READ_ERROR);
	}
	for (int i = 0; i < n; i++)
		mem[_setdma + i] = buf[i];
	_sec++;
	return status(OK);
}

uint8_t Disk::write(Memory &mem) {

	if (!seek()) {
		ERR("disk: seek error");
		return status(SEEK_ERROR);
	}

	uint8_t buf[dp->seclen];
	for (unsigned i = 0; i < sizeof(buf); i++)
		buf[i] = mem[_setdma + i];

	int n = _write(buf, sizeof(buf));
	if (n < 0 || (unsigned)n != sizeof(buf)) {
		ERR("disk: write error");
		return status(WRITE_ERROR);
	}
	_sec++;
	return status(OK);
}

uint8_t Disk::track(uint8_t a) {

	if (a >= dp->tracks) {
		ERR("disk: settrk: %d", a);
		return status(ILLEGAL_TRACK);
	}
	_settrk = a;
	return status(OK);
}

uint8_t Disk::sector(uint16_t a) {

	if (a > dp->sectrk) {
		ERR("disk: setsec: %d", a);
		return status(ILLEGAL_SECTOR);
	}
	_setsec = a;
	return status(OK);
}
