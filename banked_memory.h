#if !defined(__BANKED_MEMORY_H__)
#define __BANKED_MEMORY_H__

// see z80pack/cpmsim/srcsim/memsim.c
#define DEFAULT_BANK_SIZE (48 * 1024)

class BankedMemory: public Memory {
public:
	virtual Device *get(address at) const;

	void begin(uint8_t nbanks);

	uint8_t num_banks() const { return _nbanks; }

	void select(uint8_t bank) { _bank = bank; }

	uint8_t selected() const { return _bank; }

	void bank_size(uint8_t pages) { _bank_size = (pages << 8); }

	uint8_t bank_size() const { return _bank_size >> 8; }

	void wp_common(uint8_t wp);

	uint8_t wp_common() const;

	void set_wp_fault_handler(std::function<void(void)> fn);

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
	uint8_t _bank, _nbanks;

	uint16_t _bank_size = DEFAULT_BANK_SIZE;
};

#endif
