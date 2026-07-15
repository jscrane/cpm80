#pragma once

class Console {
public:
	void reset();

	uint8_t poll();
	uint8_t available();

	void write(uint8_t);
};
