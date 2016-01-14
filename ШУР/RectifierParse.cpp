#include "RectifierParser.h"

RectifierParser::RectifierParser()
	:serial_("COM8"),
	parser_(nullptr)
{
	try {
		serial_.setBaudRate(4800);
		// TEST 0x7e 0x01 0x01 0x03 0x05 0x0d
		//const std::string s = fromHex("7e010103050d");
	}
	catch (...) {
		throw;
	}
}

size_t RectifierParser::send(std::string command) {
	return serial_.writeSome(command);
}

RectifierParser::~RectifierParser() {
	serial_.stop();

	delete parser_;
	parser_ = nullptr;
}

const std::string getTwoByteBcdFromDouble(double value, int multiply) {
	std::string res;
	value *= multiply;
	int seconsPart = (int)value % 100;
	int firstPart = ((int)value - seconsPart) / 100;
	res += decToBcd(firstPart);
	res += decToBcd(seconsPart);
	return res;
}

const std::string RectifierParser::buildSetVoltageAndCurrentMessage(uint8_t address, double volts, double amps) {
	/*if (volts > MAX_VOLTS) volts = MAX_VOLTS;
	if (volts < MIN_VOLTS) volts = MIN_VOLTS;*/
	if (amps > MAX_AMPS) amps = MAX_AMPS;
	if (amps < MIN_AMPS) amps = MIN_AMPS;
	std::string infoPart;
	infoPart += getTwoByteBcdFromDouble(volts, 10);
	infoPart += getTwoByteBcdFromDouble(amps, 10);
	return buildSimpleIssuedMessage(RectifierOrder::ISSUED_SET_OUTPUT_PARAMETERS, address, infoPart);
}

const std::string RectifierParser::buildControlOnOffMessage(uint8_t address, bool isOn) {
	std::string infoPart;
	infoPart += decToBcd(isOn ? 0x0U : 0x01U);
	infoPart += decToBcd(0x00U);
	return buildSimpleIssuedMessage(RectifierOrder::ISSUED_CONTROL_ON_OFF, address, infoPart);
}

const std::string RectifierParser::buildSimpleIssuedMessage(uint8_t cid, uint8_t address, std::string info) {
	std::string result;
	result += decToBcd(address);
	result += decToBcd(info.size() + 1);
	result += cid;
	result += info;
	uint8_t chksum = getCheckSum(result.c_str(), result.size());
	chksum = decToBcd(chksum);
	result += chksum;
	return (char)START_FRAME_BYTE + result + (char)END_FRAME_BYTE;
}

uint8_t RectifierParser::parse() {
	try {
		uint16_t bytesRead = 0;
		// get minimum bytes with message length
		const char* data = serial_.getBytes(RECTIFIER_MIN_LENGTH, bytesRead);
		checkByte(0x7e, data, bytesRead);
		BUFFER_READ_BCD(frame_.address, data, bytesRead);
		BUFFER_READ_BCD(frame_.length, data, bytesRead);

		// Remaining number of bytes 
		uint16_t nBytes = frame_.length + RECTIFIER_CS_TAIL_LENGTH - bytesRead;
		// if not received the full data, then nBytes > 0
		serial_.getBytes(nBytes, bytesRead);

		BUFFER_READ(frame_.cid, data, bytesRead);
		RectifierInfoParser::makeRectifierInfoParser(frame_.cid, parser_);
		parser_->parseInfo(data, bytesRead, frame_.length);
		pParsedMesage_ = parser_->getData();
		return parser_->getCid();
	}
	catch (...)
	{
		throw;
	}
}

void* RectifierParser::getData() {
	return pParsedMesage_;
}