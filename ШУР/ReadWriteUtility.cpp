#include "ReadWriteUtility.h"

void checkByte(uint8_t expected, const char*& data, uint16_t& length) {
	if (length < 1U || expected != *data)
		throw "checkByte Error";
	data++;
	length--;
}

int bcdToInt(uint16_t n, uint8_t* data) {
	int uResult = 0;
	uint16_t i;

	for (i = 0; i < n; i++)	{
		uResult = (uResult * 100) + ((data[i] >> 4) * 10) + (data[i] & 0x0F);
	}
	return uResult;
}

int readFromBufLe(uint8_t* pValueArray, uint16_t valueSize, const char*& data, uint16_t& length, bool isBcd) {
	// For BCD conversion
	uint8_t* pOldValueArray = pValueArray;
	uint16_t oldValueSize = valueSize;

	for (; valueSize != 0U; valueSize--) {
		if (length-- < 1U) {
			return -1;
		}
		*pValueArray++ = *(data++);
	}

	// convert if BCD
	if (isBcd) {
		int a = bcdToInt(oldValueSize, pOldValueArray);
		memcpy(pOldValueArray, &a, oldValueSize);
	}
	return 0;
}

uint8_t decToBcd(uint8_t dec) {
	return (dec / 10) * 16 + (dec % 10);
}