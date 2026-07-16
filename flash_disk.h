#pragma once

#include "disk.h"

class FlashDisk: public Disk {
public:
	void reset() override;
	uint8_t select(uint8_t) override;

	bool checkpoint() override;
	bool restore() override;

protected:
	bool _seek(long pos) override;
	int _read(uint8_t *buf, size_t len) override;
	int _write(const uint8_t *buf, size_t len) override;
};
