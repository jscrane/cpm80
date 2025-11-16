#include <Arduino.h>

#include "memory.h"
#include "debugging.h"
#include "banked_memory.h"

static class WriteProtect: public Memory::Device {
public:
	WriteProtect(): Memory::Device(0), _wp_common(0) {}

	virtual void access(Memory::address addr) { _protect->access(addr); }

	virtual void operator=(uint8_t b) {

		if (_wp_common)
			_wp_common |= 0x80;
		else
			(*_protect) = b;
	}

	virtual operator uint8_t() { return (uint8_t)(*_protect); }

private:
	friend class BankedMemory;

	uint8_t _wp_common;

	Device *_protect;
} wp;

uint8_t BankedMemory::wp_common() const { return wp._wp_common; }

void BankedMemory::wp_common(uint8_t b) { wp._wp_common = b; }

static BankedMemory::Bank **banks;

Memory::Device *BankedMemory::get(address addr) const {

	if (addr < _bank_size)
		return _bank > 0? banks[_bank]: Memory::get(addr);

	wp._protect = Memory::get(addr);
	return &wp;
}

void BankedMemory::begin(uint8_t nbanks) {

	DBG_MEM(printf("%d banks\r\n", nbanks));

	_nbanks = nbanks;
	banks = new BankedMemory::Bank*[nbanks];
	for (int i = 1; i < nbanks; i++)
		banks[i] = new Bank(_bank_size);
}

BankedMemory::Bank::Bank(unsigned bytes): Memory::Device(bytes) {

	DBG_MEM(printf("new bank %d bytes\r\n", bytes));

	_mem = (uint8_t *)malloc(bytes);
	if (!_mem)
		ERR(printf("malloc %d failed\r\n", bytes));
}

BankedMemory::Bank::~Bank() {
	free(_mem);
}
