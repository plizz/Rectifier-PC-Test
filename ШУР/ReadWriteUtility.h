#pragma once

#include <cstdint>
#include <cstring>


void checkByte(uint8_t expected, const char*& data, uint16_t& length);

int bcdToInt(uint16_t n, uint8_t* data);
uint8_t decToBcd(uint8_t dec);

// Arguments: pValueArray - pointer to value
//            valueSize - value size in bytes
//            data - reference to buffer begin
//            length - buffer size
//            isBcd - BCD conversion flag
// Return:    zero on success, nonzero on error
int readFromBufLe(uint8_t* pValueArray, uint16_t valueSize, const char*& data, uint16_t& length, bool isBcd = false);

#define BUFFER_READ(value, buffer, length) if (0 != readFromBufLe((uint8_t*)&value, (uint16_t)sizeof(value), data, length)) { throw "Read from buffer Error"; }
#define BUFFER_READ_BCD(value, buffer, length) if (0 != readFromBufLe((uint8_t*)&value, (uint16_t)sizeof(value), data, length, true)) { throw "Read from buffer (BCD) Error"; }
