#include "RectifierInfoParse.h"

void RectifierInfoParser::makeRectifierInfoParser(uint8_t cid, RectifierInfoParser*& rectifierInfoParser) {
	// delete previous object
	delete rectifierInfoParser;
	rectifierInfoParser = nullptr;

	switch (cid)
	{
	case RectifierOrder::UPLOAD_BOARD_REGISTRATION:
		rectifierInfoParser = new RectifierBoardRegistrationParser();
		break;
	case RectifierOrder::UPLOAD_VERSION_NUMBER:
		rectifierInfoParser = new RectifierVersionNumberParser();
		break;
	case RectifierOrder::UPLOAD_GET_OUTPUT_PARAMETERS:
		rectifierInfoParser = new RectifierGetOutputParametersParser();
		break;
	case RectifierOrder::UPLOAD_CONTROL_ON_OFF:
		rectifierInfoParser = new RectifierControlOnOffParser();
		break;
	case RectifierOrder::UPLOAD_GET_RECTIFIER_SERIES:
		rectifierInfoParser = new RectifierGetSeriesParser();
		break;
	case RectifierOrder::UPLOAD_SET_OUTPUT_PARAMETERS:
		rectifierInfoParser = new RectifierSetOutputParametersParser();
		break;
	case RectifierOrder::UPLOAD_GET_VOLTAGE_AND_CURRENT:
		rectifierInfoParser = new RectifierVoltageAndCurrentParser();
		break;
	case RectifierOrder::UPLOAD_GET_SERIAL_NUMBER:
		rectifierInfoParser = new RectifierSerialNumberParser();
		break;
	case RectifierOrder::UPLOAD_GET_INFORMATION:
		rectifierInfoParser = new RectifierInformationParser();
		break;
	default:
		throw "Unexpected cid";
		break;
	}
	rectifierInfoParser->setSid(cid);
}

void RectifierInfoParser::setSid(uint8_t cid){
	cid_ = cid;
}

uint8_t getCheckSum(const char* data, uint8_t length) {
	uint8_t sum = 0;
	for (uint8_t it = 0; it < length; it++) {
		sum += *(data + it);
	}
	return sum;
}

void checkChksumAndEndFrame(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
	if (bytesRead != 2U) throw "Wrong length";
	uint8_t length = frameLength + RECTIFIER_MIN_LENGTH - 1;
	uint8_t calcChksum = getCheckSum(data - length, length) % 100;
	uint8_t byte;
	BUFFER_READ_BCD(byte, data, bytesRead);
	if (calcChksum != byte) throw "Wrong checksum";
	checkByte(0x0d, data, bytesRead);
}

// UPLOAD_BOARD_REGISTRATION
void RectifierBoardRegistrationParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {

}

// UPLOAD_VERSION_NUMBER
void RectifierVersionNumberParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {

}

// UPLOAD_GET_OUTPUT_PARAMETERS
void RectifierGetOutputParametersParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
	try {
		uint8_t byte;
		uint16_t twoByte;
		BUFFER_READ(byte, data, bytesRead);
		outputParameters_.result = byte == 0U;
		BUFFER_READ_BCD(twoByte, data, bytesRead);
		outputParameters_.outputVoltage = twoByte / 10.0;
		BUFFER_READ_BCD(twoByte, data, bytesRead);
		outputParameters_.outputCurrent = twoByte / 100.0;
		BUFFER_READ_BCD(twoByte, data, bytesRead);
		outputParameters_.fanSpeed = twoByte / 10.0;
		BUFFER_READ_BCD(twoByte, data, bytesRead);	// 0
		// Module alarm volume 
		BUFFER_READ(byte, data, bytesRead);
		outputParameters_.isLimitFlagAlarm = byte & 0x01;
		outputParameters_.isModuleAlarm = (byte >> 1) & 0x01;
		outputParameters_.isModuleShutDown = (byte >> 2) & 0x01;
		outputParameters_.isFanAlarm = (byte >> 4) & 0x01;
		outputParameters_.isAcAlarm = (byte >> 5) & 0x01;
		outputParameters_.isModuleProtectionAlarm = (byte >> 6) & 0x01;
		// Module protection type
		BUFFER_READ(outputParameters_.moduleProtectionCode, data, bytesRead);
		checkChksumAndEndFrame(data, bytesRead, frameLength);
		pData_ = (MessageOutputParameters*)&outputParameters_;
	}
	catch (...) {
		throw;
	}
}

// UPLOAD_CONTROL_ON_OFF
void RectifierControlOnOffParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
	try {
		uint8_t byte;
		BUFFER_READ(byte, data, bytesRead);
		controlOnOff_.isOnStatus = !(byte == 0U);
		checkChksumAndEndFrame(data, bytesRead, frameLength);
		pData_ = (ControlOnOff*)&controlOnOff_;
	}
	catch (...) {
		throw;
	}
}

// UPLOAD_GET_RECTIFIER_SERIES
void RectifierGetSeriesParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {

}

void parseVoltageAndCurrent(MessageVoltageAndCurrent& messageVoltageAndCurrent, const char*& data, uint16_t& bytesRead) {
	try {
		uint8_t byte;
		uint16_t twoByte;
		BUFFER_READ(byte, data, bytesRead);
		messageVoltageAndCurrent.result = byte == 0U;
		BUFFER_READ_BCD(twoByte, data, bytesRead);
		messageVoltageAndCurrent.outputVoltage = twoByte / 10.0;
		BUFFER_READ_BCD(twoByte, data, bytesRead);
		messageVoltageAndCurrent.outputCurrent = twoByte / 100.0;
	}
	catch (...) {
		throw;
	}
}

// UPLOAD_SET_OUTPUT_PARAMETERS
void RectifierSetOutputParametersParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
	try {
		parseVoltageAndCurrent(voltageAndCurrent_, data, bytesRead);
		checkChksumAndEndFrame(data, bytesRead, frameLength);
		pData_ = (MessageVoltageAndCurrent*)&voltageAndCurrent_;
	}
	catch (...) {
		throw;
	}
}

// UPLOAD_GET_VOLTAGE_AND_CURRENT
void RectifierVoltageAndCurrentParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
	try {
		parseVoltageAndCurrent(voltageAndCurrent_, data, bytesRead);
		checkChksumAndEndFrame(data, bytesRead, frameLength);
		pData_ = (MessageVoltageAndCurrent*)&voltageAndCurrent_;
	}
	catch (...) {
		throw;
	}
}

// UPLOAD_GET_SERIAL_NUMBER
void RectifierSerialNumberParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {

}

// UPLOAD_GET_INFORMATION
void RectifierInformationParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {

}