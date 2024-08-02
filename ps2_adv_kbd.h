#ifndef __PS2_ADV_KBD_H__
#define __PS2_ADV_KBD_H__

class ps2advkbd: public kbd {
public:
	uint8_t read();
	uint8_t avail();
	void reset();
};

#endif
