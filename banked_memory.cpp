#include <Arduino.h>
#include <hardware.h>

#if defined(BOARD_HAS_PSRAM)
#include <esp32-hal-psram.h>
#endif

#include "memory.h"
#include "banked_memory.h"

static BankedMemory::Bank **banks;

Memory::Device *BankedMemory::get(address at) const {

	if (_bank == 0 || at >= _bank_size) {
//		DBG(println(at, 16));
		return Memory::get(at);
	}

	return banks[_bank];
}

void BankedMemory::begin(uint8_t nbanks) {

	DBG(printf("%d banks\r\n", nbanks));

	_nbanks = nbanks;
	banks = new BankedMemory::Bank*[nbanks];
	for (int i = 1; i < nbanks; i++)
		banks[i] = new Bank(_bank_size);
}

BankedMemory::Bank::Bank(unsigned bytes): Memory::Device(bytes) {

	DBG(printf("new bank %d bytes\r\n", bytes));

#if defined(BOARD_HAS_PSRAM)
	_mem = (uint8_t *)ps_malloc(bytes);
#else
	_mem = (uint8_t *)malloc(bytes);
#endif
	if (!_mem)
		DBG(printf("malloc %d failed\r\n", bytes));
}

BankedMemory::Bank::~Bank() {
	free(_mem);
}
