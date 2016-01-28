#ifndef H_RECTIFIER_PARSER
#define H_RECTIFIER_PARSER

#include <cstring>
#include <stdint.h>
#include "RectifierSerial.h"
#include "RectifierInfoParser.h"

#define SEND_MESSAGE_LENGTH 32

#define MIN_VOLTS	190U
#define MAX_VOLTS	290U
#define MIN_AMPS	0.1
#define MAX_AMPS	70U

struct RectifierFrame {
	uint8_t address;
	uint8_t length;
	uint8_t cid;
	void* info;

	~RectifierFrame() {
	}
};

class RectifierParser {
public:
	RectifierParser(const std::string com_port_name, uint32_t baudRate);
	~RectifierParser();
	int16_t getAnswer();
	void* getData();
	
	void send();
	// Arguments: cid - command code
	//            address - module address
	//			  length - cid + info length
	//			  info - info part of message
  //        infoLength length of info
	// Return:    void
	void buildSimpleIssuedMessage(char cid, uint8_t address, char* info = NULL, uint8_t infoLength = 0);
	// Arguments: cid - command code
	//            address - module address
	//			      isOn - true - power on; false - power off
	// Return:    void
	void buildControlOnOffMessage(uint8_t address, bool isOn);
	// Arguments: address - module address
	//			      volts
	//			      amps
	// Return:    void
	void buildSetVoltageAndCurrentMessage(uint8_t address, double volts, double amps);
  uint16_t getErrorCounter();
  uint16_t getParseCounter();
private:
  char sendMessage_[SEND_MESSAGE_LENGTH];
  uint8_t sendMessageLength_;
	RectifierSerial serial_;
	RectifierFrame frame_;
	RectifierInfoParser* parser_;
	void* pParsedMesage_;
  uint16_t errorCounter_;
  uint16_t parseCounter_;
};

#endif	// H_RECTIFIER_PARSER
