/*
==UartCommE4xx.cpp==

Copyright (c) 2015–2017 Texas Instruments Incorporated

All rights reserved not granted herein.

Limited License.

Texas Instruments Incorporated grants a world-wide, royalty-free,
non-exclusive license under copyrights and patents it now or
hereafter owns or controls to make, have made, use, import,
offer to sell and sell ("Utilize") this software subject to
the terms herein.  With respect to the foregoing patent license,
such license is granted  solely to the extent that any such patent
is necessary to Utilize the software alone.  The patent license
shall not apply to any combinations which include this software,
other than combinations with devices manufactured by or for TI
(“TI Devices”). No hardware patent is licensed hereunder.

Redistributions must preserve existing copyright notices and
reproduce this license (including the above copyright notice
and the disclaimer and (if applicable) source code license
limitations below) in the documentation and/or other materials
provided with the distribution

Redistribution and use in binary form, without modification,
are permitted provided that the following conditions are met:

*	No reverse engineering, decompilation, or disassembly of
this software is permitted with respect to any software
provided in binary form.

*	any redistribution and use are licensed by TI for use only
with TI Devices.

*	Nothing shall obligate TI to provide you with source code
for the software licensed and provided to you in object code.

If software source code is provided to you, modification and
redistribution of the source code are permitted provided that
the following conditions are met:

*	any redistribution and use of the source code, including any
resulting derivative works, are licensed by TI for use only
with TI Devices.

*	any redistribution and use of any object code compiled from
the source code and any resulting derivative works, are licensed
by TI for use only with TI Devices.

Neither the name of Texas Instruments Incorporated nor the names
of its suppliers may be used to endorse or promote products derived
from this software without specific prior written permission.

DISCLAIMER.

THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI’S LICENSORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "stdafx.h"

const uint8_t loopInitialization = 10; 
const uint16_t maxDelay = 5000; // miliseconds

#if defined(_WIN32) || defined(_WIN64)
#else
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <termios.h>    // POSIX terminal control definitions
#include <sys/ioctl.h>

struct termios2 {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_line;
	cc_t c_cc[19];
	speed_t c_ispeed;
	speed_t c_ospeed;
};
#endif
 
UartCommE4xx::UartCommE4xx(ModeParams* modeParams, Util * util)
	:port(io_service),
	timer(io_service),
	rxUartBuffer(10),
	readError(true)
{
	BslE4xxResponseHandler::util = util;
	BslE4xxResponseHandler::modeParams = modeParams;
	this->resetIdxRead();
	this->util = util;
}

UartCommE4xx::~UartCommE4xx()
{
	this->close();
}

void UartCommE4xx::init(ModeParams* modeParams)
{
	// Check if the port is open. If yes, close it before initialize the port.
	if (isOpen())
	{
		close();
	}
	const char * c = modeParams->commPort.c_str();
	port.open(c);

	// Configure the flow into NONE setting
	const boost::asio::serial_port_base::flow_control FLOW(boost::asio::serial_port_base::flow_control::none);
	port.set_option(FLOW);

	// Configure the UART with 1 STOP bit
	const boost::asio::serial_port_base::stop_bits STOP(boost::asio::serial_port_base::stop_bits::one);
	port.set_option(STOP);

	// Configure the NO-PARITY 
	const boost::asio::serial_port_base::parity PARITY(boost::asio::serial_port_base::parity::none);
	port.set_option(PARITY);

	// Configure the UART to have 8 bit data format
	port.set_option(boost::asio::serial_port_base::character_size(8));

	// If the MSP-FET is chosen to communicate with MSP432P4xx I2C and UART,
	//    MSP-FET needs to be invoked with (baud-rate+2)
	if (modeParams->useMspFet)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]MSP-FET does not support MSP432E4xx BSL!");
	}
	// MSP-BSL Rocket is expected to be used with BSL-Scripter
	//    MSP-BSL Rocket requires invocation with baudrate+8 to initialize the 
	else
	{
		uint32_t invokeBslRocketE4xx = modeParams->baudRate + INVOKE_E4xx_ROCKET_ADD;
		changeNonStandardBaudRate(invokeBslRocketE4xx);
	}

	// In I2C E4xx BSL, the custom I2C address is sent by having the 
	//    slave address in baudrate to the MSP-BSL Rocket
	if (modeParams->i2cSlaveAddr != 0)
	{
		if (modeParams->useMspFet)
		{
			throw std::runtime_error("[ERROR_MESSAGE]MSP-FET does not support the custom I2C address configuration!");
		}

		std::cout << "\tCustom I2C Address is chosen: " << util->convertUlongToString(modeParams->i2cSlaveAddr,true) << std::endl;

		uint32_t invokeCustomI2cAddress = modeParams->i2cSlaveAddr + modeParams->baudRate;
		changeNonStandardBaudRate(invokeCustomI2cAddress);
	}

	// Configure the standard baud rate for the BSL communication
	changeNonStandardBaudRate(modeParams->baudRate);

	// Keep this value of baudrate to be compared when the new baud rate is setup
	actualBaudRate = modeParams->baudRate;

	// The MSP432E4xx UART BSL requires the sync by sending "0x55" to start the BSL communication
	//   for the custom bootloader, it might be that the UART sync is not required
	if ((modeParams->protocol == Protocol::Uart) && (!modeParams->disableAutoBaud))
	{
		std::cout << "\tSend the auto-baud sync bytes!" << std::endl;
		this->transmitE4xxUartSync();
	}

	//Any serial bootloader in E4xx shall be invoked with Ping Command
	this->transmitE4xxPingCommand();
}

void UartCommE4xx::transmitE4xxUartSync()
{
	bool syncUartBsl = false;

	for (uint8_t i = 0; i < loopInitialization; i++)
	{
		boost::system::error_code ec;
		boost::asio::write(port, boost::asio::buffer(SYNC_UART_BYTES, SIZE_SYNC_UART_BYTES), ec);
		if (ec)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Error from writing occurs!");
		}
		receiveBuffer(SIZE_ACK_RESPONSE);
		if ((rxUartBuffer.at(0) == E4XX_SUCCESSFUL_OPERATION) &&
			(rxUartBuffer.at(1) == E4XX_ACK))
		{
			syncUartBsl = true;
			std::cout << "\tInitialization of E4xx BSL succeed!" << std::endl;
			break;
		}
	}

	if (!syncUartBsl)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Initialization of E4xx BSL failed! Exit the scripter!");
	}
}

void UartCommE4xx::transmitE4xxPingCommand()
{
 	bool pingE4xx = false;

	for (uint8_t i = 0; i < loopInitialization; i++)
	{
		boost::system::error_code ec;
		boost::asio::write(port, boost::asio::buffer(PING, SIZE_PING), ec);
		if (ec)
		{
			throw std::runtime_error("\t[ERROR_MESSAGE]Error from writing occurs!");
		}
		receiveBuffer(SIZE_ACK_RESPONSE);
		if ((rxUartBuffer.at(0) == E4XX_SUCCESSFUL_OPERATION) &&
			(rxUartBuffer.at(1) == E4XX_ACK))
		{
			pingE4xx = true;
			std::cout << "\tE4xx BSL is invoked!" << std::endl;
			break;
		}
	}

	if (!pingE4xx)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]Initialization of E4xx BSL failed! Exit the scripter!");
	}
}

void UartCommE4xx::transmitBuffer(std::vector<uint8_t>* txBuffer)
{
	//clear the buffer
	for (uint8_t i = 0; i < rxUartBuffer.size(); i++)
	{
		rxUartBuffer.at(i) = 0;
	}
	ack = 0xFF;
	wrapTxBuffer.clear();
	//Send the Host ACK that does not require wrapping
	if ((txBuffer->at(0) == E4XX_SUCCESSFUL_OPERATION) && 
		(txBuffer->at(1) == E4XX_ACK))
	{
		wrapTxBuffer.push_back(txBuffer->at(0));
		wrapTxBuffer.push_back(txBuffer->at(1));
	}
	//Else is to send the BSL commands
	//   The content of the  wrapTxBuffer would be:
	//   1 total byte  + 1 byte checksum  + data
	else
	{
		wrapTxBuffer.push_back(txBuffer->size() + 2); // 1 total byte
		wrapTxBuffer.push_back(0); // 1 byte checksum

		//number of data 
		util->resetCrcE4xx();
		for (uint16_t i = 0; i < txBuffer->size(); i++)
		{
			wrapTxBuffer.push_back(txBuffer->at(i));
			util->checkSumCalculateE4xx(txBuffer->at(i));
		}
		wrapTxBuffer.at(1) = util->retCrcE4xx();
	}

	needGetStatus = false;
	if ((wrapTxBuffer.at(2) == DOWNLOAD_CMD) ||
		(wrapTxBuffer.at(2) == SEND_DATA_CMD))
	{
		needGetStatus = true;
	}

	boost::system::error_code ec;
	boost::asio::write(port, boost::asio::buffer(wrapTxBuffer, wrapTxBuffer.size()), ec);
	if (ec)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]Error from writing occurs!");
	}

	if (wrapTxBuffer.at(2) == SEND_DATA_CMD)
	{
		bytesWrite += (txBuffer->size() - 1);
		bytesSent += (txBuffer->size() - 1);
	}
}

void UartCommE4xx::changeBaudRate(uint32_t baudRate)
{
	throw std::runtime_error("\t[ERROR_MESSAGE]Changes baud rate is not applicable for MSP432E4xx!")  ;
}


void UartCommE4xx::changeNonStandardBaudRate(uint32_t baudRate)
{
#if defined(_WIN32) || defined(_WIN64)

#elif defined(__APPLE__)
	int fd = port.native_handle();
#else //Ubuntu
	struct termios2 tio;
	int fd = port.native_handle();
#endif

#if defined(_WIN32) || defined(_WIN64)
	port.set_option(boost::asio::serial_port::baud_rate(baudRate));
#elif defined(__APPLE__)
	ioctl(fd, _IOW('T', 2, speed_t), baudRate, 1);
#else
	ioctl(fd, TCGETS2, &tio);
	tio.c_cflag &= ~CBAUD;
	tio.c_cflag |= 0010000;
	tio.c_ispeed = baudRate;
	tio.c_ospeed = baudRate;
	ioctl(fd, TCSETS2, &tio);
#endif

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

std::vector<uint8_t>*  UartCommE4xx::retPtrTxBuffer()
{
	for (uint8_t i = 0; i < SIZE_GET_STATUS; i++)
	{
		wrapTxBuffer.push_back(GET_STATUS[i]);
	}

	for (uint8_t i = 0; i < SIZE_ACK_RESPONSE; i++)
	{
		wrapTxBuffer.push_back(ACK_HOST[i]);
	}

	return &wrapTxBuffer;
}

std::vector<uint8_t>*  UartCommE4xx::retPtrRxBuffer()
{
	return &rxUartBuffer;
}

std::vector<uint8_t>* UartCommE4xx::receiveBuffer(int32_t size)
{
#if defined (_WIN32) || defined (_WIN64)
	while (waitReceive(0, size));
#else
	waitReceive(0, size);
#endif

	if (needGetStatus)
	{
		//Send status
		transmitE4xxGetStatus();
#if defined (_WIN32) || defined (_WIN64)
		//Get status
		while (waitReceive(SIZE_ACK_RESPONSE, (SIZE_ACK_RESPONSE + SIZE_GET_STATUS)));
#else
		waitReceive(SIZE_ACK_RESPONSE, (SIZE_ACK_RESPONSE + SIZE_GET_STATUS));
#endif
		//Send host ACK
		transmitE4xxHostAck();
		std::this_thread::sleep_for(std::chrono::microseconds(10));
		needGetStatus = false;
	}
	return &rxUartBuffer;
}

void UartCommE4xx::transmitE4xxGetStatus()
{
	boost::system::error_code ec;
	boost::asio::write(port, boost::asio::buffer(GET_STATUS, SIZE_GET_STATUS), ec);
	if (ec)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]Error from writing occurs!");
	}
}

void UartCommE4xx::transmitE4xxHostAck()
{
	boost::system::error_code ec;
	boost::asio::write(port, boost::asio::buffer(ACK_HOST, SIZE_ACK_RESPONSE), ec);
	if (ec)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]Error from writing occurs!");
	}
}

bool UartCommE4xx::waitReceive(uint16_t offset, size_t numBytes)
{
#if defined (_WIN32) || defined (_WIN64)
	port.get_io_service().reset();

	async_read(port, boost::asio::buffer(&rxUartBuffer[offset], numBytes),
			   bind(&UartCommE4xx::onRead, this, boost::asio::placeholders::error,
			   boost::asio::placeholders::bytes_transferred));

	timer.expires_from_now(boost::posix_time::millisec(maxDelay));
	timer.async_wait(boost::bind(&UartCommE4xx::onTimer,
		this,
		boost::asio::placeholders::error));

	port.get_io_service().run();
	if (!readError)
	{

	}
	return !readError;
#else
	//Start deadlinetimer
	timerEvent = false;
	if (&timer)
	{
		boost::system::error_code ec;
		timer.expires_from_now(boost::posix_time::milliseconds(maxDelay), ec);
		timer.async_wait(bind(&UartCommE4xx::onTimer, this, boost::asio::placeholders::error));
	}
	//Start Async_REad
	bytesReceived = 0;
	uint32_t actSize = 0;
	bool process = false;
	readEvent = false;
	async_read(port, boost::asio::buffer(&rxUartBuffer[offset], numBytes), bind(&UartCommE4xx::onRead, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
	
	//Wait the reading process
	boost::system::error_code ec;
	while (io_service.run_one(ec))
	{
		if (readEvent)
		{
			if (bytesReceived > 0)
			{
				actSize += bytesReceived;
				if (actSize == numBytes)
				{
					timer.cancel(ec);
					process = true;
					break;
				}
			}
		}
		else if (timerEvent)
		{
			port.cancel(ec);
			break;
		}

		if (io_service.stopped())
		{
			io_service.reset();
		}
	}
	//Let cancelled tasks finish
	io_service.run(ec);
	io_service.reset();
	return process;
#endif
}
 
void UartCommE4xx::close()
{
	if (isOpen())
	{
		boost::system::error_code ec = port.close(ec);
	}
}

void UartCommE4xx::resetBytesWritten()
{
	bytesWrite = 0;
}

uint32_t UartCommE4xx::retBytesWritten()
{
	return bytesWrite;
}

uint32_t UartCommE4xx::retBytesRead()
{
	return bytesRead;
}

void UartCommE4xx::processResponse(uint8_t ack, std::vector<uint8_t> *response)
{
	BslE4xxResponseHandler::processResponse(ack, response);
}

bool UartCommE4xx::retErrorAck()
{
	return BslE4xxResponseHandler::retErrorAck();
}

bool UartCommE4xx::retErrorStatus()
{
	return BslE4xxResponseHandler::retErrorStatus();
}

std::string UartCommE4xx::retErrorAckMessage()
{
	return BslE4xxResponseHandler::retErrorAckMessage();
}

std::string UartCommE4xx::retErrorStatusMessage()
{
	return BslE4xxResponseHandler::retErrorStatusMessage();
}

bool UartCommE4xx::retCrcCheckResult()
{
	//This function is not applicable in E4xx family, yet
	//   has to be implemented as the CommInterface implementation
	return 0;
}

std::string UartCommE4xx::translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx)
{
	//idx is not applicable to be used in E4xx as the buffer always contains one packet 
	//    while in MSP430 and P4xx, one big rxUartBuffer could contain more than one packet of BSL response
	//    so the idx is required to mark whcih packet that we are accessing
	return BslE4xxResponseHandler::translateResponse(cmdParams, buffer);
}

void UartCommE4xx::resetBytesSent()
{
	bytesSent = 0;
}

uint32_t UartCommE4xx::retBytesSent()
{
	return bytesSent;
}

void UartCommE4xx::resetIdxRead()
{
	bytesReceived = 0;
}

void UartCommE4xx::resetIdxAcc()
{
	resetIdxRead();
}

uint8_t UartCommE4xx::retAck()
{
	return rxUartBuffer.at(1);
}

uint32_t UartCommE4xx::retLastRxBufferIdx()
{
	return bytesReceived;
}

void UartCommE4xx::onTimer(const boost::system::error_code& ec)
{
#if defined (_WIN32) || defined (_WIN64)
	if (ec)
	{
		return;
	}
	//Timeout reached
	port.cancel();
	throw std::runtime_error("[ERROR_MESSAGE]No response bytes received from BSL!");
#else
	timerEvent = (ec != boost::asio::error::operation_aborted);
#endif
}

void UartCommE4xx::onRead(const boost::system::error_code& ec, size_t numBytes)
{
#if defined (_WIN32) || defined (_WIN64)
	readError = (ec || numBytes == 0);
	timer.cancel();
#else
	readEvent = (ec != boost::asio::error::operation_aborted);
	bytesReceived = numBytes;
#endif
}

bool UartCommE4xx::isOpen() const
{
	return &port && port.is_open();
}
