#ifndef __PS2_KBD_H__
#define __PS2_KBD_H__

class ps2kbd: public kbd {
public:
	uint8_t read();
	uint8_t avail();
	void reset();
private:
	bool modifier(unsigned scan, bool key_down);
	bool _shift, _ctrl;
};

#endif
