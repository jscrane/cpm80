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

// max number of entries in drivemap.txt
// hardwired into BIOS:
// ;       fixed data tables for four-drive standard
// ;       ibm-compatible 8" disks
#define FD_DRIVES	4
#define HD_DRIVES	2
#define DRIVES (FD_DRIVES + HD_DRIVES)

#define DRIVE_LETTERS	26

class Memory;

// Common CP/M-style sector I/O bookkeeping. Platform subclasses (flash_disk,
// linux_disk) only need to implement drive selection/reset and the raw
// byte-oriented primitives below (_seek/_read/_write); the track/sector
// state machine and sector-sized read/write logic is shared here.
class Disk {
public:
	virtual ~Disk() {}

	virtual void reset() =0;

	uint8_t select(uint8_t);
	uint8_t read(Memory &);
	uint8_t write(Memory &);
	bool seek();
	uint8_t track(uint8_t);
	uint8_t sector(uint16_t);
	void dma(uint16_t a) { _setdma = a; }

	uint8_t status() const { return _status; }
	uint8_t track() const { return _settrk; }
	uint16_t sector() const { return _setsec; }
	uint16_t dma() const { return _setdma; }

	// machine-snapshot-to-storage; not every platform supports this,
	// so subclasses that don't need it can just leave it unimplemented
	virtual bool checkpoint() { return false; }
	virtual bool restore() { return false; }

protected:
	int _add_drive(int drive_letter, uint32_t size);

	// raw, platform-specific byte I/O on the currently selected drive
	virtual bool _open(uint8_t drive_id) =0;
	virtual bool _seek(long pos) =0;
	virtual int _read(uint8_t *buf, size_t len) =0;
	virtual int _write(const uint8_t *buf, size_t len) =0;

private:
	uint8_t status(uint8_t s) { _status = s; return s; }

	uint8_t _settrk, _trk;
	uint16_t _setsec, _sec;
	uint16_t _setdma;
	uint8_t _status;

	// geometry of the currently selected drive
	uint8_t _tracks, _seclen;
	uint16_t _sectrk;
};
