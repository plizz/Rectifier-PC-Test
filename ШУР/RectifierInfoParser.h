#ifndef H_RECTIFIER_INFO_PARSER
#define H_RECTIFIER_INFO_PARSER

#include <stdint.h>
#include "ReadWriteUtility.h"
#include "RectifierParserDefs.h"

uint8_t getCheckSum(const char* data, uint8_t length);

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
	static bool makeRectifierInfoParser(int16_t cid, RectifierInfoParser*& rectifierInfoParser);
	// parse info method. Implemented in the derived class
	virtual int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) { return RectifierParseState::OK; };
	void* getData() { return pData_; };
	int16_t getCid() { return cid_; };
private:
	void setSid(int16_t cid);
	int16_t cid_;
protected:
	void* pData_;
};

class RectifierBoardRegistrationParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};

class RectifierVersionNumberParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageVersionInfo versionInfo_;
};

class RectifierGetOutputParametersParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageOutputParameters outputParameters_;
};

class RectifierControlOnOffParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	ControlOnOff controlOnOff_;
};

class RectifierGetSeriesParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};

class RectifierSetOutputParametersParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageVoltageAndCurrent voltageAndCurrent_;
};

class RectifierVoltageAndCurrentParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
private:
	MessageVoltageAndCurrent voltageAndCurrent_;
};

class RectifierSerialNumberParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};

class RectifierInformationParser : public RectifierInfoParser {
public:
	int8_t parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength);
};

#endif // H_RECTIFIER_INFO_PARSER
