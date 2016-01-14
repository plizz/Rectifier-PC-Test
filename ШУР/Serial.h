#pragma once

#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>

using std::vector;

std::string fromHex(std::string hex);

#define SERIAL_PORT_READ_BUF_SIZE 256
#define SERIAL_TIMEOUT_MS 600

class Serial {
private:
	const std::string comPortName_;
	boost::shared_ptr<boost::asio::serial_port> port_;
	boost::asio::io_service io_service_;
	boost::asio::deadline_timer timeout_;
	char readBuffer_[SERIAL_PORT_READ_BUF_SIZE];

	void init();
	int writeSome(const char *buf, const int &size);
public:
	Serial(const std::string com_port_name);
	void stop();
	int writeSome(const std::string& buf);
	const char* getBytes(unsigned int nBytes, uint16_t& bytesRead);
	void setBaudRate(int);
	void setDataBitsSize(int);
	void setParity(boost::asio::serial_port_base::parity::type);
	void setStopBits(boost::asio::serial_port_base::stop_bits::type);
};