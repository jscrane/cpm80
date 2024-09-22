#ifndef __IO_H__
#define __IO_H__

class serial_kbd;

// input ports: A = IN(n)
// (see https://st.sdf-eu.org/i8080/index.html)
#define CON_ST		0
#define CON_IN		1
#define FDC_GETTRK	11
#define FDC_GETSEC_L	12
#define FDC_IODONE	13
#define FDC_STATUS	14

// output ports: OUT(n, A)
#define CON_OUT		1
#define FDC_SELDSK	10
#define FDC_SETTRK	11
#define FDC_SETSEC_L	12
#define FDC_IO		13
#define FDC_SETDMA_L	15
#define FDC_SETDMA_H	16
#define FDC_SETSEC_H	17
#define MEM_INIT	20
#define MEM_SELECT	21
#define MEM_SIZE	22

// disk errors
#define OK		0
#define ILLEGAL_DRIVE	1
#define ILLEGAL_TRACK	2
#define ILLEGAL_SECTOR	3
#define SEEK_ERROR	4
#define READ_ERROR	5
#define WRITE_ERROR	6
#define ILLEGAL_CMD	7

class BankedMemory;

class IO: public PortDevice, public Display {
public:
	IO(BankedMemory &mem, serial_kbd &kbd, serial_dsp &dsp): _mem(mem), _kbd(kbd), _dsp(dsp) {}

	uint8_t in(uint16_t p);
	void out(uint16_t p, uint8_t b);

	void reset();
private:
	serial_kbd &_kbd;
	uint8_t kbd_poll();

	void dsk_reset();
	uint8_t dsk_read();
	uint8_t dsk_write();
	bool dsk_seek();
	uint8_t dsk_select(uint8_t a);
	uint8_t dsk_settrk(uint8_t a);
	uint8_t dsk_setsec(uint8_t a);
	uint8_t settrk, setsec, trk, sec;
	uint16_t setdma;
	uint8_t dsk_status;
	BankedMemory &_mem;

	serial_dsp &_dsp;
};

#endif
