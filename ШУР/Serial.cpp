#include "Serial.h"

std::string fromHex(std::string hex) {
	int len = hex.length();
	std::string newString;
	for (int i = 0; i < len; i += 2) {
		std::string byte = hex.substr(i, 2);
		char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
		newString.push_back(chr);
	}
	return newString;
}

Serial::Serial(const std::string comPortName)
	: comPortName_(comPortName),
	io_service_(),
	timeout_(io_service_)
{
	init();
}

void Serial::init() {
	port_ = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(io_service_));
	boost::system::error_code ec;
	try {
		port_->open(comPortName_);
	}
	catch (boost::system::system_error error) {
		throw;
	}
	setBaudRate(9600);
	setDataBitsSize(8);
	setStopBits(boost::asio::serial_port_base::stop_bits::one);
	setParity(boost::asio::serial_port_base::parity::none);
}

void Serial::stop()
{
	if (port_) {
		port_->cancel();
		port_->close();
		port_.reset();
	}
	io_service_.stop();
	io_service_.reset();
}

const char* Serial::getBytes(unsigned int nBytes, uint16_t& bytesRead) {
	bool readError = false;
	// After a timeout & cancel it seems we need
	// to do a reset for subsequent reads to work.
	port_->get_io_service().reset();
	timeout_.expires_from_now(boost::posix_time::milliseconds(SERIAL_TIMEOUT_MS));
	timeout_.async_wait([this](const boost::system::error_code& error) 
	{
		if (!error)	{
			// Timer expired.
			port_->cancel();
		}
	});
	boost::asio::async_read(*port_, boost::asio::buffer(readBuffer_ + bytesRead, SERIAL_PORT_READ_BUF_SIZE), boost::asio::transfer_at_least(nBytes),
		[this, &readError, &bytesRead](const boost::system::error_code& error, std::size_t bytesTransferred)
	{
		readError = !!error;
		bytesRead += bytesTransferred;
		timeout_.cancel();
	});
	// This will block until a character is read
	// or until the it is cancelled.
	port_->get_io_service().run();
	if (readError) throw "Read from port error";
	return readBuffer_;
}

int Serial::writeSome(const std::string &buf) {
	return writeSome(buf.c_str(), buf.size());
}

int Serial::writeSome(const char *buf, const int &size) {
	boost::system::error_code ec;

	if (!port_) return -1;
	if (size == 0) return 0;

	return port_->write_some(boost::asio::buffer(buf, size), ec);
}

void Serial::setBaudRate(int baudRate){
	port_->set_option(boost::asio::serial_port_base::baud_rate(baudRate));
}

void Serial::setDataBitsSize(int dataBitsSize){
	port_->set_option(boost::asio::serial_port_base::character_size(dataBitsSize));
}

void Serial::setParity(boost::asio::serial_port_base::parity::type parity) {
	port_->set_option(boost::asio::serial_port_base::parity(parity));
}

void Serial::setStopBits(boost::asio::serial_port_base::stop_bits::type stopBits) {
	port_->set_option(boost::asio::serial_port_base::stop_bits(stopBits));
}