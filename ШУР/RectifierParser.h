#pragma once

#include "Serial.h"
#include <cmath>
#include "ReadWriteUtility.h"
#include "RectifierInfoParse.h"

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
	RectifierParser();
	~RectifierParser();
	uint8_t parse();
	void* getData();
	
	size_t send(std::string command);
	// Arguments: cid - command code
	//            address - module address
	//			  length - cid + info length
	//			  info info part of message
	// Return:    command string
	const std::string buildSimpleIssuedMessage(uint8_t cid, uint8_t address, std::string info = "");
	// Arguments: cid - command code
	//            address - module address
	//			  isOn - true - power on; false - power off
	// Return:    command string
	const std::string buildControlOnOffMessage(uint8_t address, bool isOn);
	// Arguments: address - module address
	//			  volts
	//			  amps
	// Return:    command string
	const std::string buildSetVoltageAndCurrentMessage(uint8_t address, double volts, double amps);
private:
	Serial serial_;
	RectifierFrame frame_;
	RectifierInfoParser* parser_;
	void* pParsedMesage_;
};