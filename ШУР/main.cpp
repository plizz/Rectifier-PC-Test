//#include "vld.h"

#include "RectifierParser.h"
#include <iostream>

RectifierParser rectifierParser("COM7", 4800);

void updateVoltAndCur(uint8_t address) {
	rectifierParser.buildSimpleIssuedMessage(ISSUED_GET_OUTPUT_PARAMETERS, address);
	rectifierParser.send();
	int16_t cid = rectifierParser.getAnswer();
	if (cid == RectifierOrder::UPLOAD_GET_OUTPUT_PARAMETERS) {
		MessageOutputParameters* message = (MessageOutputParameters*)rectifierParser.getData();
		std::cout << "Rectifier[" << (int)address << "] V = " << message->outputVoltage << "\tA = " << message->outputCurrent << std::endl;
	}
	else {
		std::cout << "Rectifier[" << (int)address << "] Error. Error code = " << cid << std::endl;
	}
}

int main(int argc, char* argv[])
{
	while (true) {
		updateVoltAndCur(1);
		updateVoltAndCur(2);
		Sleep(50);
	}

	return 0;
}