#if !defined(__BANKED_MEMORY_H__)
#define __BANKED_MEMORY_H__

// see z80pack/cpmsim/srcsim/memsim.c
#define DEFAULT_BANK_SIZE (48 * 1024)

class BankedMemory: public Memory {
public:
	virtual Device *get(address at) const;

	void begin(uint8_t nbanks);

	void select(uint8_t bank) { _bank = bank; }

	void size(uint8_t pages) { _bank_size = pages * 256; }

	class Bank: public Memory::Device {
	public:
		Bank(unsigned bytes);
		virtual ~Bank();

		virtual void operator=(uint8_t b) { _mem[_acc] = b; }
		virtual operator uint8_t() { return _mem[_acc]; }

	private:
		uint8_t *_mem;
	};

private:
	uint8_t _bank;
	uint16_t _bank_size = DEFAULT_BANK_SIZE;
};

#endif
