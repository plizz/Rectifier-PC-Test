#include "RectifierInfoParser.h"

bool RectifierInfoParser::makeRectifierInfoParser(int16_t cid, RectifierInfoParser*& rectifierInfoParser) {
  bool result = true;
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
		result = false;
		break;
	}
	rectifierInfoParser->setSid(cid);
  return result;
}

void RectifierInfoParser::setSid(int16_t cid){
	cid_ = cid;
}

uint8_t getCheckSum(const char* data, uint8_t length) {
	uint16_t sum = 0;
	for (uint8_t it = 0; it < length; it++) {
		sum += (uint8_t)*(data + it);
    // TODO Check this!
    if (sum > MAX_CHKSUM_NUMBER) sum -= MAX_CHKSUM_NUMBER;
	}
	return (uint8_t)(sum % 100);
}

int8_t checkChksumAndEndFrame(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
	if (bytesRead != 2U) return RectifierParseState::WRONG_LENGTH_ERROR;  
	uint8_t length = frameLength + RECTIFIER_MIN_LENGTH - 1;
	uint8_t calcChksum = getCheckSum(data - length, length);
	uint8_t oneByte;
	BUFFER_READ_BCD(oneByte, data, bytesRead);
	if (calcChksum != oneByte)  {
        return RectifierParseState::WRONG_CHK_SUM_ERROR;  
	}
	if(!checkByte(END_FRAME_BYTE, data, bytesRead)) return RectifierParseState::CHECK_START_TAIL_BYTE_ERROR;
    return RectifierParseState::OK;
}

// UPLOAD_BOARD_REGISTRATION
int8_t RectifierBoardRegistrationParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    return RectifierParseState::OK;
}

// UPLOAD_VERSION_NUMBER
int8_t RectifierVersionNumberParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    return RectifierParseState::OK;
}

// UPLOAD_GET_OUTPUT_PARAMETERS
int8_t RectifierGetOutputParametersParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
  uint8_t oneByte;
  uint16_t twoByte;
  BUFFER_READ(oneByte, data, bytesRead);
  outputParameters_.result = oneByte == 0U;
  
  BUFFER_READ_BCD(twoByte, data, bytesRead);
  outputParameters_.outputVoltage = twoByte / 10.0;
  
  BUFFER_READ_BCD(twoByte, data, bytesRead);
  outputParameters_.outputCurrent = twoByte / 100.0;
  
  BUFFER_READ_BCD(twoByte, data, bytesRead);
  outputParameters_.fanSpeed = twoByte / 10.0;
  
  BUFFER_READ_BCD(twoByte, data, bytesRead);	// 0
  // Module alarm volume 
  BUFFER_READ(oneByte, data, bytesRead);
  outputParameters_.isLimitFlagAlarm = oneByte & 0x01;
  outputParameters_.isModuleAlarm = (oneByte >> 1) & 0x01;
  outputParameters_.isModuleShutDown = (oneByte >> 2) & 0x01;
  outputParameters_.isFanAlarm = (oneByte >> 4) & 0x01;
  outputParameters_.isAcAlarm = (oneByte >> 5) & 0x01;
  outputParameters_.isModuleProtectionAlarm = (oneByte >> 6) & 0x01;  
  // Module protection type 
  BUFFER_READ(outputParameters_.moduleProtectionCode, data, bytesRead);
  int8_t checkResult = checkChksumAndEndFrame(data, bytesRead, frameLength);
  if (RectifierParseState::OK == checkResult) {
      pData_ = (MessageOutputParameters*)&outputParameters_;
      return RectifierParseState::OK;
  } else {
      return checkResult;
  }
}

// UPLOAD_CONTROL_ON_OFF
int8_t RectifierControlOnOffParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    uint8_t oneByte;
    BUFFER_READ(oneByte, data, bytesRead);
    controlOnOff_.isOnStatus = !(oneByte == 0U);
    int8_t checkResult = checkChksumAndEndFrame(data, bytesRead, frameLength);
    if (RectifierParseState::OK == checkResult) {
        pData_ = (ControlOnOff*)&controlOnOff_;
        return true;
    } else {
        return checkResult;
    }
}

// UPLOAD_GET_RECTIFIER_SERIES
int8_t RectifierGetSeriesParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    return RectifierParseState::OK;
}

int8_t parseVoltageAndCurrent(MessageVoltageAndCurrent& messageVoltageAndCurrent, const char*& data, uint16_t& bytesRead) {
    uint8_t byte;
    uint16_t twoByte;
    BUFFER_READ(byte, data, bytesRead);
    messageVoltageAndCurrent.result = byte == 0U;
    BUFFER_READ_BCD(twoByte, data, bytesRead);
    messageVoltageAndCurrent.outputVoltage = twoByte / 10.0;
    BUFFER_READ_BCD(twoByte, data, bytesRead);
    messageVoltageAndCurrent.outputCurrent = twoByte / 100.0;
    return RectifierParseState::OK;
}

// UPLOAD_SET_OUTPUT_PARAMETERS
int8_t RectifierSetOutputParametersParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    int8_t parseVCResult = parseVoltageAndCurrent(voltageAndCurrent_, data, bytesRead);
    if (RectifierParseState::OK != parseVCResult) return parseVCResult;
    int8_t checkResult = checkChksumAndEndFrame(data, bytesRead, frameLength);
    if (RectifierParseState::OK == checkResult) {
        pData_ = (MessageVoltageAndCurrent*)&voltageAndCurrent_;
        return true;
    } else {
       return checkResult;
    }
}

// UPLOAD_GET_VOLTAGE_AND_CURRENT
int8_t RectifierVoltageAndCurrentParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    int8_t parseVCResult = parseVoltageAndCurrent(voltageAndCurrent_, data, bytesRead);
    if (RectifierParseState::OK != parseVCResult) return parseVCResult;
    int8_t checkResult = checkChksumAndEndFrame(data, bytesRead, frameLength);
    if (RectifierParseState::OK == checkResult) {
         pData_ = (MessageVoltageAndCurrent*)&voltageAndCurrent_;
        return true;
    } else {
        return checkResult;
    }
}

// UPLOAD_GET_SERIAL_NUMBER
int8_t RectifierSerialNumberParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    return RectifierParseState::OK;
}

// UPLOAD_GET_INFORMATION
int8_t RectifierInformationParser::parseInfo(const char*& data, uint16_t& bytesRead, uint8_t frameLength) {
    return RectifierParseState::OK;
}
