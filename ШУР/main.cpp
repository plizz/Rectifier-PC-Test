#include <vld.h> 

#include "RectifierParser.h"
#include <iostream>

int main(int argc, char* argv[])
{
	MessageOutputParameters* message;
	uint8_t cid;
	RectifierParser rectifierParser;
	bool isRestart = false;
	try {
		const std::string setParameters = rectifierParser.buildSetVoltageAndCurrentMessage(1U, 220, 0.5);
		rectifierParser.send(setParameters/*.substr(3,5)*/);
		cid = rectifierParser.parse();
		const std::string commandGetParameters = rectifierParser.buildSimpleIssuedMessage(RectifierOrder::ISSUED_GET_VOLTAGE_AND_CURRENT, 1U);
		rectifierParser.send(commandGetParameters);
		cid = rectifierParser.parse();
		if (cid == RectifierOrder::UPLOAD_GET_VOLTAGE_AND_CURRENT) {
			MessageVoltageAndCurrent* message = (MessageVoltageAndCurrent*)rectifierParser.getData();
			std::cout << "Set succes! : " << message->outputVoltage << " -- " << message->outputCurrent << std::endl;
		}

		if (isRestart) {
			std::string controlCommand = rectifierParser.buildControlOnOffMessage(1U, false);
			rectifierParser.send(controlCommand);
			cid = rectifierParser.parse();
			if (cid == RectifierOrder::UPLOAD_CONTROL_ON_OFF) {
				ControlOnOff* message = (ControlOnOff*)rectifierParser.getData();
				std::cout << "ON OFF succes! : " << message->isOnStatus << std::endl;
			}
			Sleep(3000);
			controlCommand = rectifierParser.buildControlOnOffMessage(1U, true);
			rectifierParser.send(controlCommand);
			cid = rectifierParser.parse();
			if (cid == RectifierOrder::UPLOAD_CONTROL_ON_OFF) {
				ControlOnOff* message = (ControlOnOff*)rectifierParser.getData();
				std::cout << "ON OFF succes! : " << message->isOnStatus << std::endl;
			}
		}
	}
	catch (const char* ex) {
		std::cout << "----------------TEST----------------" << std::endl << ex << std::endl;
	}

	while (/*int i = 0; i < 10; i++*/ true) {
		try {
			const std::string commandGetVoltage = rectifierParser.buildSimpleIssuedMessage(RectifierOrder::ISSUED_GET_OUTPUT_PARAMETERS, 1U);
			rectifierParser.send(commandGetVoltage);
			cid = rectifierParser.parse();
			if (cid == RectifierOrder::UPLOAD_GET_OUTPUT_PARAMETERS) {
				message = (MessageOutputParameters*)rectifierParser.getData();
				std::cout << message->outputVoltage << " -- " << message->outputCurrent << std::endl;
			}
		}
		catch (const char* ex) {
			std::cout << /*"----------------TEST----------------" << */std::endl << ex << std::endl;
		}
		Sleep(1000);
	}


	/*catch (const std::exception&) {

	}
	catch (const char* ex) {
		std::cout << "----------------TEST----------------" << std::endl << ex << std::endl;
	}
	catch (...) {
		std::cout << "----------------TEST----------------" << std::endl << "UNHANDLED ERROR" << std::endl;
	}*/

	return 0;
}