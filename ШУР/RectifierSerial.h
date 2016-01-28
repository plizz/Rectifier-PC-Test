#ifndef H_RECTIFIER_SERIAL
#define H_RECTIFIER_SERIAL

#include "RectifierParserDefs.h"
#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>

#define SERIAL_TIMEOUT_MS 150
#define SERIAL_PORT_READ_BUF_SIZE 256

class RectifierSerial {
public:
	RectifierSerial(const std::string com_port_name);
	~RectifierSerial();
    void begin(uint32_t baudRate); 
    void write(char* buf, uint8_t length);
    int16_t receiveRxBuffer();
    const char* getBuffer();
private:
	const std::string comPortName_;
	boost::shared_ptr<boost::asio::serial_port> port_;
	boost::asio::io_service io_service_;
	boost::asio::deadline_timer timeout_;
    char readBuffer_[SERIAL_PORT_READ_BUF_SIZE];
};

#endif      // H_RECTIFIER_SERIAL
