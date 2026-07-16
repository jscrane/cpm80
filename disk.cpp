#include <stdint.h>
#include <stddef.h>

#include <memory.h>
#include <debugging.h>

#include "disk.h"

bool Disk::seek() {

	if (_trk != _settrk || _sec != _setsec) {
		_trk = _settrk;
		_sec = _setsec;
		return _seek((long)_seclen * (_sectrk * _trk + _sec - 1));
	}
	return true;
}

uint8_t Disk::read(Memory &mem) {

	if (!seek()) {
		ERR("disk: seek error");
		return status(SEEK_ERROR);
	}

	uint8_t buf[_seclen];
	int n = _read(buf, _seclen);
	if (n < 0 || (unsigned)n != _seclen) {
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

	uint8_t buf[_seclen];
	for (unsigned i = 0; i < _seclen; i++)
		buf[i] = mem[_setdma + i];

	int n = _write(buf, _seclen);
	if (n < 0 || (unsigned)n != _seclen) {
		ERR("disk: write error");
		return status(WRITE_ERROR);
	}
	_sec++;
	return status(OK);
}

uint8_t Disk::track(uint8_t a) {

	if (a >= _tracks) {
		ERR("disk: settrk: %d", a);
		return status(ILLEGAL_TRACK);
	}
	_settrk = a;
	return status(OK);
}

uint8_t Disk::sector(uint16_t a) {

	if (a > _sectrk) {
		ERR("disk: setsec: %d", a);
		return status(ILLEGAL_SECTOR);
	}
	_setsec = a;
	return status(OK);
}
