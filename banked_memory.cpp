#include <Arduino.h>

#include "memory.h"
#include "debugging.h"
#include "banked_memory.h"

static BankedMemory::Bank **banks;

Memory::Device *BankedMemory::get(address at) const {

	if (_bank == 0 || at >= _bank_size)
		return Memory::get(at);

	return banks[_bank];
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
