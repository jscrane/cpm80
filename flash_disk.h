#pragma once

#include "disk.h"

class FlashDisk: public Disk {
public:
	void reset() override;
	bool checkpoint() override;
	bool restore() override;

protected:
	bool _open(uint8_t drive_id) override;
	bool _seek(long pos) override;
	int _read(uint8_t *buf, size_t len) override;
	int _write(const uint8_t *buf, size_t len) override;
};
