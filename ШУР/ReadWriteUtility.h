#ifndef H_READ_WRITE_UTILITY
#define H_READ_WRITE_UTILITY

#include <cstring>
#include <stdint.h>
#include "RectifierParserDefs.h"

bool checkByte(uint8_t expected, const char*& data, uint16_t& length);

int bcdToInt(uint16_t n, uint8_t* data);
char decToBcd(uint8_t dec);

// Arguments: pValueArray - pointer to value
//            valueSize - value size in bytes
//            data - reference to buffer begin
//            length - buffer size
//            isBcd - BCD conversion flag
// Return:    zero on success, nonzero on error
int readFromBufLe(uint8_t* pValueArray, uint16_t valueSize, const char*& data, uint16_t& length, bool isBcd = false);

#define BUFFER_READ(value, buffer, length) if (0 != readFromBufLe((uint8_t*)&value, (uint16_t)sizeof(value), data, length)) { return RectifierParseState::READ_ERROR; }
#define BUFFER_READ_BCD(value, buffer, length) if (0 != readFromBufLe((uint8_t*)&value, (uint16_t)sizeof(value), data, length, true)) { return RectifierParseState::READ_ERROR; }


#endif H_READ_WRITE_UTILITY
