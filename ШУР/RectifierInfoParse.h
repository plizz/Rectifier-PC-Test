#pragma once

#include "ReadWriteUtility.h"

#include <iostream>

// SOI		Start flag		1 byte
// ADDR		Device address	1 byte
// LENGTH					1 byte
#define RECTIFIER_MIN_LENGTH 3

// CHKSUM	Checksum		1 byte
// EOI		End of frame	1 byte
#define RECTIFIER_CS_TAIL_LENGTH 2

#define START_FRAME_BYTE 0x7e
#define END_FRAME_BYTE 0x0d

uint8_t getCheckSum(const char* data, uint8_t length);

enum RectifierOrder {
	ISSUED_BOARD_REGISTRATION = 0x01,		// Board Registration Response	
	UPLOAD_BOARD_REGISTRATION = 0x81,		// Board Registration Response
	ISSUED_VERSION_NUMBER = 0x02,			// Get the version number	
	UPLOAD_VERSION_NUMBER = 0x82,			// Get the version number
	ISSUED_GET_OUTPUT_PARAMETERS = 0x03,	// Get the rectifier output voltage, current, alarm volume	
	UPLOAD_GET_OUTPUT_PARAMETERS = 0x83,	// Get the rectifier output voltage, current, alarm volume
	ISSUED_CONTROL_ON_OFF = 0x04,			// Control ON / OFF	
	UPLOAD_CONTROL_ON_OFF = 0x84,			// Control ON / OFF
	ISSUED_GET_RECTIFIER_SERIES = 0x05,		// Get rectifier module series	
	UPLOAD_GET_RECTIFIER_SERIES = 0x85,		// Get rectifier module series
	ISSUED_SET_OUTPUT_PARAMETERS = 0x06,	// Set the rectifier output voltage, current	
	UPLOAD_SET_OUTPUT_PARAMETERS = 0x86,	// Set the rectifier output voltage, current
	ISSUED_GET_VOLTAGE_AND_CURRENT = 0x07,	// Get rectifier voltage and current set parameters	
	UPLOAD_GET_VOLTAGE_AND_CURRENT = 0x87,	// Get rectifier voltage and current set parameters
	ISSUED_GET_SERIAL_NUMBER = 0X10,		// Get module serial number	
	UPLOAD_GET_SERIAL_NUMBER = 0X90,		// Get module serial number	
	ISSUED_GET_INFORMATION = 0X11,			// Get module manufacturers information	
	UPLOAD_GET_INFORMATION = 0X91			// Get module manufacturers information
};

enum ModuleProtectionCode {
	OVER_TEMPERATURE = 1U,
	OVER_VOLTAGE,
	SHORT_CIRCUIT
};

class MessageVersionInfo {
public:
	bool result;
	uint8_t softMajorVersion;
	uint8_t softMinorVersion;
	uint8_t hardPcbVersion;
	uint8_t hardProgDeviceVersion;
	uint8_t ModelType;
};

class MessageOutputParameters {
public:
	bool result;
	double outputVoltage;
	double outputCurrent;
	double fanSpeed;
	bool isModuleProtectionAlarm;
	bool isAcAlarm;
	bool isFanAlarm;
	bool isModuleShutDown;
	bool isModuleAlarm;
	bool isLimitFlagAlarm;
	uint8_t moduleProtectionCode;
};

class ControlOnOff {
public:
	bool result;
	bool isOnStatus;
};

class MessageVoltageAndCurrent {
public:
	bool result;
	double outputVoltage;
	double outputCurrent;
};

// Base class for parser
class RectifierInfoParser {
public:
	// Make desired object and return pointer to it
	static void makeRectifierInfoParser(uint8_t cid, RectifierInfoParser*& rectifierInfoParser);
	// parse info method. Implemented in the derived class
	virtual void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {};
	void* getData() { return pData_; };
	uint8_t getCid() { return cid_; };
private:
	void setSid(uint8_t cid);
	uint8_t cid_;
protected:
	void* pData_;
};

class RectifierBoardRegistrationParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};

class RectifierVersionNumberParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageVersionInfo versionInfo_;
};

class RectifierGetOutputParametersParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageOutputParameters outputParameters_;
};

class RectifierControlOnOffParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	ControlOnOff controlOnOff_;
};

class RectifierGetSeriesParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};

class RectifierSetOutputParametersParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageVoltageAndCurrent voltageAndCurrent_;
};

class RectifierVoltageAndCurrentParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageVoltageAndCurrent voltageAndCurrent_;
};

class RectifierSerialNumberParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};

class RectifierInformationParser : public RectifierInfoParser {
public:
	void parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};