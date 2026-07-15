#pragma once

// disk errors
#define OK		0
#define ILLEGAL_DRIVE	1
#define ILLEGAL_TRACK	2
#define ILLEGAL_SECTOR	3
#define SEEK_ERROR	4
#define READ_ERROR	5
#define WRITE_ERROR	6
#define ILLEGAL_CMD	7

class Memory;

class Disk {
public:
	void reset();
	uint8_t read(Memory &);
	uint8_t write(Memory &);
	bool seek();
	uint8_t select(uint8_t);
	uint8_t track(uint8_t);
	uint8_t sector(uint16_t);
	void dma(uint16_t a) { _setdma = a; }

	uint8_t status() const { return _status; }
	uint8_t track() const { return _settrk; }
	uint16_t sector() const { return _setsec; }
	uint16_t dma() const { return _setdma; }

	bool checkpoint();
	bool restore();

private:
	uint8_t status(uint8_t s) { _status = s; return s; }

	uint8_t _settrk, _trk;
	uint16_t _setsec, _sec;
	uint16_t _setdma;
	uint8_t _status;
};
