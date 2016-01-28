#include "RectifierParser.h"

RectifierParser::RectifierParser(const std::string com_port_name, uint32_t baudRate)
	:serial_(com_port_name),
	parser_(nullptr),
	errorCounter_(0),
	parseCounter_(0) 
{ 
	  serial_.begin(baudRate);
}

void RectifierParser::send() {
	serial_.write(sendMessage_, sendMessageLength_);
}

RectifierParser::~RectifierParser() {
	delete parser_;
	parser_ = nullptr;
}

void getTwoByteBcdFromDouble(char* data, uint8_t& pos, double value, int multiply) {
  value *= multiply;
  int seconsPart = (int)value % 100;
  int firstPart = ((int)value - seconsPart) / 100;
  *(data + pos) = decToBcd(firstPart);
  pos++;
  *(data + pos) = decToBcd(seconsPart);
  pos++;
}

void RectifierParser::buildSetVoltageAndCurrentMessage(uint8_t address, double volts, double amps) {
  if (volts > MAX_VOLTS) volts = MAX_VOLTS;
  if (volts < MIN_VOLTS) volts = MIN_VOLTS;
  if (amps > MAX_AMPS) amps = MAX_AMPS;
  if (amps < MIN_AMPS) amps = MIN_AMPS;
  char infoPart[4];
  uint8_t pos = 0;
  getTwoByteBcdFromDouble(infoPart, pos, volts, 10);
  getTwoByteBcdFromDouble(infoPart, pos, amps, 10);
  
  buildSimpleIssuedMessage(RectifierOrder::ISSUED_SET_OUTPUT_PARAMETERS, address, infoPart, sizeof(infoPart));
}

void RectifierParser::buildControlOnOffMessage(uint8_t address, bool isOn) {
  char infoPart[2];
  uint8_t pos = 0;
  infoPart[pos++] = decToBcd(isOn ? 0x0U : 0x01U);
  infoPart[pos++] = decToBcd(0x00U);
	buildSimpleIssuedMessage(RectifierOrder::ISSUED_CONTROL_ON_OFF, address, infoPart, pos);
}

void RectifierParser::buildSimpleIssuedMessage(char cid, uint8_t address, char* info, uint8_t infoLength) {
  sendMessageLength_ = 0;
  sendMessage_[sendMessageLength_++] = (char)START_FRAME_BYTE;
  sendMessage_[sendMessageLength_++] = decToBcd(address);
  sendMessage_[sendMessageLength_++] = decToBcd(infoLength + 1);
  sendMessage_[sendMessageLength_++] = cid;
  memcpy(sendMessage_ + sendMessageLength_, info, infoLength);
  sendMessageLength_ += infoLength;
	uint8_t chksum = getCheckSum(sendMessage_ + 1, sendMessageLength_ - 1);
  sendMessage_[sendMessageLength_++] = decToBcd(chksum);
  sendMessage_[sendMessageLength_++] = (char)END_FRAME_BYTE;
}

int16_t RectifierParser::getAnswer() {
  errorCounter_++;
  parseCounter_++;
  int16_t receiveRxBufferResult = serial_.receiveRxBuffer();
  // get minimum bytes with message length
  if (receiveRxBufferResult < 0) {
    return receiveRxBufferResult; // Error code, if receiveRxBuffer() get error
  }
  uint16_t bytesRead = (uint16_t)receiveRxBufferResult;
  if (bytesRead < RECTIFIER_MIN_LENGTH) {
    return RectifierParseState::GET_MINIMAL_FRAME_ERROR;
  }
  const char* data = serial_.getBuffer();
  
  if (!checkByte(0x7e, data, bytesRead))  {
    return RectifierParseState::CHECK_START_TAIL_BYTE_ERROR;
  }
  BUFFER_READ_BCD(frame_.address, data, bytesRead);
  BUFFER_READ_BCD(frame_.length, data, bytesRead);
  if (frame_.length < (bytesRead - RECTIFIER_CS_TAIL_LENGTH))    {
    return RectifierParseState::GET_MINIMAL_FRAME_ERROR;
  }
 
  //data = serial_.getBuffer();
  BUFFER_READ(frame_.cid, data, bytesRead);

  if (!RectifierInfoParser::makeRectifierInfoParser((int16_t)frame_.cid, parser_)) 
    return RectifierParseState::MAKE_INFO_PARSER_ERROR;
  int8_t parserResult = parser_->parseInfo(data, bytesRead, frame_.length);
  if (RectifierParseState::OK != parserResult) return parserResult;
  pParsedMesage_ = parser_->getData();
  errorCounter_--;
  return parser_->getCid();
}

uint16_t RectifierParser::getErrorCounter() {
  return errorCounter_;
}

uint16_t RectifierParser::getParseCounter() {
  return parseCounter_;
}

void* RectifierParser::getData() {
	return pParsedMesage_;
}
