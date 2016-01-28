#include "RectifierSerial.h"

RectifierSerial::RectifierSerial(const std::string comPortName)
	: comPortName_(comPortName),
	io_service_(),
	timeout_(io_service_)
{ }

RectifierSerial::~RectifierSerial() {
	if (port_) {
		port_->cancel();
		port_->close();
		port_.reset();
	}
	io_service_.stop();
	io_service_.reset();
}

void RectifierSerial::begin(uint32_t baudRate) {
	port_ = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(io_service_));
	boost::system::error_code ec;
	try {
		port_->open(comPortName_);
	}
	catch (boost::system::system_error error) {
		throw;
	}
	port_->set_option(boost::asio::serial_port_base::baud_rate(baudRate));
	port_->set_option(boost::asio::serial_port_base::character_size(8));
	port_->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
	port_->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
}
    
void RectifierSerial::write(char* buf, uint8_t length) {
	port_->write_some(boost::asio::buffer(buf, length));
}

const char* RectifierSerial::getBuffer() {
  return &(readBuffer_[0]);
}
    
int16_t RectifierSerial::receiveRxBuffer() {
	bool readError = false;
	int16_t bytesRead = 0;
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
	port_->async_read_some(boost::asio::buffer(readBuffer_, SERIAL_PORT_READ_BUF_SIZE), [this, &readError, &bytesRead](const boost::system::error_code& error, std::size_t bytesTransferred)
	{
		readError = !!error;
		bytesRead = bytesTransferred;
		timeout_.cancel();
	});
	// This will block until a character is read
	// or until the it is cancelled.
	port_->get_io_service().run();
	if (readError) RectifierParseState::READ_TIMEOUT_ERROR;
	return bytesRead;
}
