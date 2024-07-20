#ifndef __SERIAL_KBD_H__
#define __SERIAL_KBD_H__

class serialkbd: public kbd {
public:
	serialkbd(HardwareSerial &serial): _serial(serial) {}

	uint8_t read();
	uint8_t avail();
	void reset();

private:
	HardwareSerial &_serial;
};

#endif
