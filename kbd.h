#if !defined(__KBD_H__)
#define __KBD_H__

class kbd {
public:
	virtual uint8_t read() = 0;
	virtual uint8_t avail() = 0;
	virtual void reset() {}
};

#endif
