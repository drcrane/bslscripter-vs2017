/*
==UartComm.cpp==

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

UartComm::UartComm(ModeParams* modeParams, Util * util)
	:port(io_service),
	timer(io_service),
	rxUartBuffer(67100)
{
	BslResponseHandler::util = util;
	BslResponseHandler::modeParams = modeParams;
	BslResponseHandler::initToggleStatus();
	this->resetIdxRead();
	this->util = util;
}

UartComm::~UartComm()
{
	this->close();
}

#define PLATFORM_DELAY(millis) std::this_thread::sleep_for(std::chrono::milliseconds(millis))

void UartComm::init(ModeParams* modeParams)
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

	// Configure the PARITY based on the parameter given in the script
	if (modeParams->enParity)
	{
		const boost::asio::serial_port_base::parity PARITY(boost::asio::serial_port_base::parity::even);
		port.set_option(PARITY);
	}
	else
	{
		const boost::asio::serial_port_base::parity PARITY(boost::asio::serial_port_base::parity::none);
		port.set_option(PARITY);
	}

	// Configure the UART to have 8 bit data format
	port.set_option(boost::asio::serial_port_base::character_size(8));

	if (modeParams->enInvoke) {
		PLATFORM_DELAY(15);

		port.set_option(RESETControl(1));
		port.set_option(TESTControl(0));
		PLATFORM_DELAY(15);

		port.set_option(TESTControl(1));
		PLATFORM_DELAY(15);

		port.set_option(TESTControl(0));
		PLATFORM_DELAY(15);

		port.set_option(TESTControl(1));
		PLATFORM_DELAY(15);

		port.set_option(RESETControl(0));
		PLATFORM_DELAY(15);

		port.set_option(TESTControl(0));
		PLATFORM_DELAY(15);
	}

	// Execute the invoke sequence for MSP430 devices
	if (modeParams->family != Family::P4xx)
	{
		uint32_t invokeMSP430 = modeParams->baudRate + INVOKE_430_BSL_ADD;
		changeNonStandardBaudRate(invokeMSP430);
	}

	// If the MSP-FET is chosen to communicate with MSP432P4xx I2C and UART,
	//    MSP-FET needs to be invoked with (baud-rate+2)
	if ((modeParams->useMspFet)	&& (modeParams->family == Family::P4xx))
	{
		uint32_t invokeMSPFET; 
		if (modeParams->protocol == Protocol::I2c)
		{
			invokeMSPFET = modeParams->baudRate + INVOKE_MSPFET_P432_BSL_I2C_ADD;
		}
		else if (modeParams->protocol == Protocol::Uart)
		{
			invokeMSPFET = INVOKE_MSPFET_P432_BSL_UART;
		}
		else
		{
			throw std::runtime_error("\t[ERROR_MESSAGE]MSP-FET does not support the SPI communication!");
		}
		changeNonStandardBaudRate(invokeMSPFET);
	}

	// In I2C , the custom I2C address is sent by having the 
	//    slave address in baudrate to the MSP-BSL Rocket
	if (modeParams->i2cSlaveAddr != 0)
	{
		if (modeParams->useMspFet)
		{
			throw std::runtime_error("[ERROR_MESSAGE]MSP-FET does not support the custom I2C address configuration!");
		}
  		std::cout << "\tCustom I2C Address is chosen!" << std::endl;
		uint32_t invokeCustomI2cAddress = modeParams->i2cSlaveAddr + modeParams->baudRate;
		changeNonStandardBaudRate(invokeCustomI2cAddress);
	}

	// Configure the standard baud rate for the BSL communication
	changeNonStandardBaudRate(modeParams->baudRate);

	// Keep this value of baudrate to be compared when the new baud rate is setup
	actualBaudRate = modeParams->baudRate;

	// The MSP432P4xx UART BSL requires the sync by sending "0xFF" to start the BSL communication
	if ((modeParams->family == Family::P4xx) &&
		(modeParams->protocol == Protocol::Uart) &&
		(!modeParams->disableAutoBaud)) 
	{
		this->transmitP4xxInitialization();
	}
}

void UartComm::transmitP4xxInitialization()
{
	const uint8_t loopInitialization = 10;
	bool syncUartBsl = false;
	uint8_t syncBytes[1] = { 0xFF };

	for (uint8_t i = 0; i < loopInitialization; i++)
	{
		ack = 0xFF;
		boost::system::error_code ec;
		boost::asio::write(port, boost::asio::buffer(syncBytes, 1), ec);
		if (ec)
		{
			throw std::runtime_error("\t[ERROR_MESSAGE]Error from writing occurs!");
		}
		receiveBuffer(-1);
		if (ack == 0x00)
		{
			syncUartBsl = true;
			std::cout << "\tInitialization of P4xx BSL succeed!" << std::endl;
			break;
		}
	}

	if (!syncUartBsl)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]Initialization of P4xx BSL failed! Exit the scripter!");
	}
}

void UartComm::transmitBuffer(std::vector<uint8_t>* txBuffer)
{
	uint32_t minData = 0;

	ack = 0xFF;
	wrapTxBuffer.clear();
	wrapTxBuffer.push_back(0x80); 
	wrapTxBuffer.push_back(txBuffer->size() & 0xFF);
	wrapTxBuffer.push_back((txBuffer->size() >> 8) & 0xFF);

	if (txBuffer->at(0) == 0x20)
	{
		minData = 5;
	}
	else
	{
		minData = 4;
	}

	util->resetCrc();
	for (uint16_t i = 0; i < txBuffer->size(); i++)
	{
		wrapTxBuffer.push_back(txBuffer->at(i));
		util->checkSumCalculate(txBuffer->at(i));
	}

	wrapTxBuffer.push_back(util->getCheckSumLow());
	wrapTxBuffer.push_back(util->getCheckSumHigh());

	boost::system::error_code ec;
	if (modeParams->family == Family::F543x)
	{
		for (uint16_t i = 0; i < wrapTxBuffer.size(); i++)
		{
			sendByte(&wrapTxBuffer.at(i));
		}
	}
	else
	{
		boost::asio::write(port, boost::asio::buffer(wrapTxBuffer, wrapTxBuffer.size()), ec);
		if (ec)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Error from writing occurs!");
		}
	}
	bytesWrite += (txBuffer->size() - minData);
	bytesSent += (txBuffer->size() - minData);
 }

void UartComm::changeBaudRate(uint32_t baudRate)
{
	port.set_option(boost::asio::serial_port::baud_rate(baudRate));
 	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void UartComm::changeNonStandardBaudRate(uint32_t baudRate)
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
  
std::vector<uint8_t>*  UartComm::retPtrTxBuffer()
{
	return &wrapTxBuffer;
}

std::vector<uint8_t>*  UartComm::retPtrRxBuffer()
{
	return &rxUartBuffer;
}

std::vector<uint8_t>* UartComm::receiveBuffer(int32_t size)
{
	uint32_t msDelay = 1000;
	if (!isOpen())
	{
		return 0;
	}

 	//start the first reading for ack
	this->setTimer(msDelay);
	this->startReadAck(&ack, expSize);

	boost::system::error_code ec;
 	while (io_service.run_one(ec))
	{
		if (readEvent)
		{
			if (bytesReceived > 0)
			{
				actSize += bytesReceived;
				if (actSize == expSize)
				{
					bool protocolFine = fsmReceiveBuffer(size);
					// all data received, state machine goes back to waitAck
					if ((waitAck) || !protocolFine)
					{
						timer.cancel(ec);
						break;
					}
				}
			}

			if (waitAck)
			{
				this->startReadAck(&ack, expSize - actSize);
			}
			else if (waitHeader)
			{
				this->startReadBuf(0, expSize - actSize);
			}
			else
			{
				this->startReadBuf(3, expSize - actSize);
			}
		}
		else if (timerEvent)
		{
			cancelled = false;
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
	return &rxUartBuffer;
}

void UartComm::close()
{
	if (isOpen())
	{
		boost::system::error_code ec = port.close(ec);
	}
}

void UartComm::resetBytesWritten()
{
	bytesWrite = 0;
}

uint32_t UartComm::retBytesWritten()
{
	return bytesWrite;
}

void UartComm::resetBytesSent()
{
	bytesSent = 0;
}

uint32_t UartComm::retBytesSent()
{
	return bytesSent;
}

uint32_t UartComm::retBytesRead()
{
	return pureData;
}

void UartComm::processResponse(uint8_t ack, std::vector<uint8_t> *response)
{
	BslResponseHandler::processResponse(ack, response);
}

bool UartComm::retErrorAck()
{
	return BslResponseHandler::retErrorAck();
}

bool UartComm::retErrorStatus()
{
	return BslResponseHandler::retErrorStatus();
}

std::string UartComm::retErrorAckMessage()
{
	return BslResponseHandler::retErrorAckMessage();
}

std::string UartComm::retErrorStatusMessage()
{
	return BslResponseHandler::retErrorStatusMessage();
}

bool UartComm::retCrcCheckResult()
{
	return BslResponseHandler::retCrcCheckResult();
}

std::string UartComm::translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx)
{
	return BslResponseHandler::translateResponse(cmdParams, buffer, idx);
}

void UartComm::sendByte(uint8_t *data)
{
	boost::system::error_code ec;
	boost::asio::write(port, boost::asio::buffer(data, 1), ec);
	std::this_thread::sleep_for(std::chrono::microseconds(100));
}

inline bool UartComm::fsmReceiveBuffer(int32_t size)
{
	bool retVal = true;

	if (size == -1)
	{
		waitAck = true;
		waitHeader = false;
		waitData = false;
		actSize = 0;
		//renew expSize for reading the header bytes
		expSize = 1;
	}
	else if (size > 0)
	{
		if (waitAck)
		{
			//these conditions are for the normal wrapping bsl packet
			if (ack == 0x00)
			{
				waitAck = false;
				waitHeader = true;
				waitData = false;
				actSize = 0;
				//renew expSize for reading the header bytes
				expSize = 3;
				cnt--;
			}
			else
			{
				retVal = false;
			}

		}
		else if (waitHeader)
		{
			if (rxUartBuffer.at(cycle * 262) == 0x80)
			{
				waitAck = false;
				waitHeader = false;
				waitData = true;
				actSize = 0;
				//renew expSize for reading the  data bytes
				expSize = (uint32_t)(((((uint16_t)rxUartBuffer.at(2 + (cycle * 262))) << 8) | rxUartBuffer.at(1 + (cycle * 262))) + 2);
			}
			else
			{
				retVal = false;
			}
		}
		else if (waitData)
		{
			waitAck = false;
			waitHeader = false;
			waitData = true;

			//crc calculation
			uint16_t crcBsl = (((uint16_t)rxUartBuffer.at(cnt - 1) << 8) |	rxUartBuffer.at(cnt - 2));
			util->resetCrc();

			for (uint32_t i = (cnt - expSize); i < (cnt - 2); i++)
			{
				util->checkSumCalculate(rxUartBuffer.at(i));
			}

			if (crcBsl != util->retCrc())
			{
				retVal = false;
			}

			//update idx
			cycle++;
			pureData += (expSize - 3);

			waitAck = true;
			waitHeader = false;
			waitData = false;
			actSize = 0;
			//renew expSize for reading again the ack
			expSize = 1;
		}
	}
	return retVal;
}

void UartComm::resetIdxRead()
{
	actSize = 0;
	cnt = 0;
	bytesReceived = 0;
	expSize = 1;
	waitAck = true;
	waitHeader = false;
	waitData = false;
}

void UartComm::resetIdxAcc()
{
	resetIdxRead();
	pureData = 0;
	cnt = 0;
	cycle = 0;
}

uint8_t UartComm::retAck()
{
	return ack;
}

uint32_t UartComm::retLastRxBufferIdx()
{
	return cnt;
}

void UartComm::setTimer(uint32_t duration)
{
	timerEvent = false;

	if (&timer)
	{
		boost::system::error_code ec;
		timer.expires_from_now(boost::posix_time::milliseconds(duration), ec);
		timer.async_wait(bind(&UartComm::onTimer, this, boost::asio::placeholders::error));
	}
}

void UartComm::onTimer(const boost::system::error_code& ec)
{
	timerEvent = (ec != boost::asio::error::operation_aborted);
}

void UartComm::startReadAck(uint8_t* buf, size_t numBytes)
{
	bytesReceived = 0;
	readEvent = false;
	async_read(port, boost::asio::buffer(buf, numBytes), bind(&UartComm::onRead, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void UartComm::startReadBuf(uint16_t offset, size_t numBytes)
{
	bytesReceived = 0;
	readEvent = false;
	async_read(port, boost::asio::buffer(&rxUartBuffer[offset + (cycle * 262)], numBytes), bind(&UartComm::onRead, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void UartComm::onRead(const boost::system::error_code& ec, size_t numBytes)
{
	readEvent = (ec != boost::asio::error::operation_aborted);
	bytesReceived = numBytes;
	cnt += bytesReceived;
}
			  
bool UartComm::isOpen() const
{
	return &port && port.is_open();
}
  
void UartComm::cancel()
{
	cancelled = true;
	boost::system::error_code ec;
	if (&timer && timer.expires_from_now(boost::posix_time::milliseconds(0), ec) > 0)
	{
		timer.async_wait(bind(&UartComm::onTimer, this, boost::asio::placeholders::error));
	}
}
  







