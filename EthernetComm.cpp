/*
==EthernetComm.cpp==

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

#if defined (_WIN32) || defined (_WIN64)

#else
	#include <netdb.h>
	#include <ifaddrs.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif

const uint8_t EthernetComm::NUM_OF_TRY_WAITING_BOOTP_OR_TFTP = 5;
const uint8_t EthernetComm::TIMER_DEADLINE = 5; 

EthernetComm::EthernetComm(ModeParams* modeParams, Util *util) :
	socketWol(this->io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), PORT_WOL_MAGIC_PACKET)),
	socketBootp(this->io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), BOOTP_SERVER_PORT)),
	socketTftpRequest(this->io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), TFTP_PORT)),
	socketTftpData(this->io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), TFTP_PORT_AUTOMATIC)),
	timerEvent(false),
	readError(true),
	timer(this->io_service),
	dataRxBuffer(MAX_SIZE_SOCKET_BUFFER),
	dataTxBuffer(MAX_SIZE_SOCKET_BUFFER) 
{
	this->modeParams = modeParams;
	this->util = util;
	this->clientIpAddress = modeParams->clientIpAddress;

	if (util->compareString(modeParams->serverIpAddress, "0.0.0.0"))
	{
		std::cout << "\tReading the Local IP Address from the system" << std::endl;
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
		boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;

		ipAddressProperties.clear();
#if defined (_WIN32) || defined (_WIN64)
		while (iter != end)
		{
			boost::asio::ip::tcp::endpoint ep = *iter++;
			boost::asio::ip::address addr = ep.address();
			ipAddressProperties.push_back(addr.to_string());
			uint8_t found = addr.to_string().find(".");
			if (found != std::string::npos)
			{
				ipv4Address = addr.to_string();
			}
		}
#else
		struct ifaddrs *ifaddr, *ifa;
		char host[NI_MAXHOST];

		if (getifaddrs(&ifaddr) == -1) {
			perror("getifaddrs");
			exit(EXIT_FAILURE);
		}

		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
		{
			if (ifa->ifa_addr == NULL)
			{
				continue;
			}

			if (ifa->ifa_addr->sa_family == AF_INET) 
			{
				std::cout <<	ifa->ifa_name   << std::endl;
				if (getnameinfo(ifa->ifa_addr,
					sizeof(struct sockaddr_in),
					host, NI_MAXHOST,
					NULL, 0, NI_NUMERICHOST) != 0) 
				{
					exit(EXIT_FAILURE);
				}

				//Get the first IP that is not "lo", it could be the en0 for Linux and
				//   eth0 for MAC. If there is more than one IP (for example, WLAN and 
				//   Ethernet LAN available), user shall configure the Local IP that
				//   would be connected using "IPServer=xxx.xxx.xxx.xxx" in the
				//   MODE scipt parameter
				if(!util->compareString(ifa->ifa_name,"lo"))
				{
					ipv4Address = host;
				}
			}
		}
		freeifaddrs(ifaddr);
#endif
	}
	else
	{
		std::cout << "\tLocal IP Address is given in the parameter"<< std::endl;
		ipv4Address = modeParams->serverIpAddress;
	}

	std::cout << "\tLocal IP Address of Host:  " << ipv4Address << std::endl;
	convertMacIpAddrStringToByte(modeParams->macAddress);
	prepareWolMagicPacket();
	resetIdxAcc();
}

EthernetComm::~EthernetComm()
{
	this->close();
}

void EthernetComm::init(ModeParams* modeParams)
{
	io_service.run();

	bool bootpRequest = false;
	repeatingWaitingRequest = 0;
	//As long the BOOTP request has not been received yet, send the WOL
	while (!bootpRequest)
	{
		//Send the WOL magic packet
		sendWolMagicPacket();
		//Receive BOOTP request
		bootpRequest = receiveBootpRequest();
		//add delay before sending next WoL Magic Packet
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	//Send BOOTP reply and wait for TFTP read request
	prepareBootpReply(&clientIpAddressInBytes, &serverIpAddressInBytes);
	repeatingWaitingRequest = 0;
	bool tftpRequest = false;
	while (!tftpRequest)
	{
		//Send BOOTP reply
		sendBootpReply();
		//Receive TFTP read request
		tftpRequest = receiveTftpReadRequest();
		//add delay before sending next BOOTP reply
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	//Get the port TFTP ID from TFTP request endpoint
	tftpSendpDataEndpointPort = tftpRequestEndpoint.port();
	std::cout << "\tGet the port of TFTP request endpoint: " << util->convertUlongToString(tftpSendpDataEndpointPort,false) << std::endl;
}

void EthernetComm::transmitBuffer(std::vector<uint8_t>* txBuffer)
{
	uint8_t* ptrTxData = &dataTxBuffer[0];

	*ptrTxData++ = (TftpProperties::TFTP_DATA >> 8) & 0x00FF;
	*ptrTxData++ = TftpProperties::TFTP_DATA & 0x00FF;

	for (uint16_t i = 0; i < txBuffer->size(); i++)
	{
		*ptrTxData++ = txBuffer->at(i);
	}

	//Calculate only the number of data from the firmware image without  TFTP_DATA header and block number
	bytesWrite += txBuffer->size() - 2;
	bytesSent += txBuffer->size() - 2;

	//Clean the rxbuffer to get actual data
	for (int16_t i = 0; i < 4; i++)
	{
		dataRxBuffer.at(i) = 0;
	}

	tftpSendpDataEndpoint.port(tftpSendpDataEndpointPort);
	tftpSendpDataEndpoint.address(boost::asio::ip::address_v4::from_string(clientIpAddress));
	socketTftpData.send_to(boost::asio::buffer(dataTxBuffer, txBuffer->size() + 2), tftpSendpDataEndpoint);
}

std::vector<uint8_t>* EthernetComm::receiveBuffer(int32_t size)
{
	socketTftpData.get_io_service().reset();
	socketTftpData.async_receive_from(boost::asio::buffer(dataRxBuffer), tftpRecpDataEndpoint,
		                              boost::bind(&EthernetComm::onRead, this,
		                              boost::asio::placeholders::error,
		                              boost::asio::placeholders::bytes_transferred));
	timerEvent = false;
	timer.expires_from_now(boost::posix_time::seconds(TIMER_DEADLINE));
	timer.async_wait(boost::bind(&EthernetComm::onTimerSocketTftpAck,
		             this,
		             boost::asio::placeholders::error));

	while (io_service.run_one(error))
	{
		if (!readError)
		{
			bytesRead = 4;
			repeatingWaitingRequest = 0;
		}

		if (timerEvent)
		{
			timerEvent = false;
			socketTftpData.cancel();
			break;
		}
	}
	io_service.run(error);
	io_service.reset();
	return &dataRxBuffer;
}

void EthernetComm::changeBaudRate(uint32_t baudRate)
{
	throw std::runtime_error("[ERROR MESSAGE]Change baud rate is not applicable for Ethernet communication!");
}

void EthernetComm::close()
{
	if (socketWol.is_open())
	{
		socketWol.close();
	}

	if (socketBootp.is_open())
	{
		socketBootp.close();
	}

	if (socketTftpRequest.is_open())
	{
		socketTftpRequest.close();
	}

	if (socketTftpData.is_open())
	{
		socketTftpData.close();
	}
	io_service.stop();
}

void EthernetComm::resetBytesWritten()
{
	bytesWrite = 0;
}

uint32_t EthernetComm::retBytesWritten()
{
	return bytesWrite;
}

void EthernetComm::resetBytesSent()
{
	bytesSent = 0;
}

uint32_t EthernetComm::retBytesSent()
{
	return bytesSent;
}

uint32_t EthernetComm::retBytesRead()
{
	return bytesRead;
}

uint8_t EthernetComm::retAck()
{
	return ack;
}

uint32_t EthernetComm::retLastRxBufferIdx()
{
	return bytesRead;
}

std::vector<uint8_t>* EthernetComm::retPtrRxBuffer()
{
	return &dataRxBuffer;
}

std::vector<uint8_t>* EthernetComm::retPtrTxBuffer()
{
	return &dataTxBuffer;
}

void EthernetComm::resetIdxAcc()
{
	bytesRead = 0;
	bytesSent = 0;
	bytesWrite = 0;
}

void EthernetComm::processResponse(uint8_t ack, std::vector<uint8_t> *response)
{
	BslE4xxEthernetResponseHandler::processResponse(ack, response);
}

bool EthernetComm::retErrorAck()
{
	return errorAck;
}

bool EthernetComm::retErrorStatus()
{
	return errorStatus;
}

std::string EthernetComm::retErrorAckMessage()
{
	return errorAckMessage;
}

std::string EthernetComm::retErrorStatusMessage()
{
	return errorStatusMessage;
}

bool EthernetComm::retCrcCheckResult()
{
	return false;
}

std::string EthernetComm::translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx)
{
	return "";
}

void EthernetComm::prepareWolMagicPacket()
{
	wolMagicPacket.clear();
	for (uint8_t i = 0; i < BootpProperties::SIZE_HW_ADDRESS; i++)
	{
		wolMagicPacket.push_back(WakeOnLan::WOL_MAGIC_PACKET_MARKER);
	}

	for (uint8_t i = 0; i < WakeOnLan::MAC_REP_WOL_MAGIC_PACKET; i++)
	{
		for (uint8_t j = 0; j < BootpProperties::SIZE_HW_ADDRESS; j++)
		{
			wolMagicPacket.push_back(macAddress[j]);
		}
	}
}

void EthernetComm::sendWolMagicPacket()
{
	socketWol.set_option(boost::asio::ip::udp::socket::reuse_address(true));
	socketWol.set_option(boost::asio::socket_base::broadcast(true));

	for (uint16_t i = 0; i < wolMagicPacket.size(); i++)
	{
		dataTxBuffer.at(i) = wolMagicPacket.at(i);
	}

	boost::asio::ip::udp::endpoint wolMagicPacketEndpoint(boost::asio::ip::address_v4::broadcast(), PORT_WOL_MAGIC_PACKET);

	socketWol.send_to(boost::asio::buffer(dataTxBuffer, wolMagicPacket.size()), wolMagicPacketEndpoint);
}

bool EthernetComm::receiveBootpRequest()
{
	bool receiveStatus = false;
	boost::asio::ip::udp::endpoint requestBootpEndpoint;
	requestBootpEndpoint.port(BOOTP_SERVER_PORT);

	boost::system::error_code error;
	socketBootp.get_io_service().reset();
	socketBootp.async_receive_from(boost::asio::buffer(dataRxBuffer), requestBootpEndpoint,
								   boost::bind(&EthernetComm::onRead, this,
								   boost::asio::placeholders::error,
								   boost::asio::placeholders::bytes_transferred)); 
	timerEvent = false;
	timer.expires_from_now(boost::posix_time::seconds(TIMER_DEADLINE));
	timer.async_wait(boost::bind(&EthernetComm::onTimerSocketBootp,
					 this,
					 boost::asio::placeholders::error)); 

	while (io_service.run_one(error))
	{
		if (!readError)
		{
			if (verifyBootpRequest(&dataRxBuffer[0]))
			{
				std::cout << "\tBOOTP request received!" << std::endl;
				repeatingWaitingRequest = 0;
				receiveStatus = true;
			}
			break;
		}
		
		if (timerEvent)
		{
			timerEvent = false;
			socketBootp.cancel();
			break;
		}
	}
	io_service.run(error);
	io_service.reset();

	return receiveStatus;
}

void EthernetComm::sendBootpReply()
{
	replyBootpEndpoint.address(boost::asio::ip::address_v4::broadcast());
	replyBootpEndpoint.port(BOOTP_CLIENT_PORT);

	socketBootp.set_option(boost::asio::socket_base::broadcast(true));
	socketBootp.send_to(boost::asio::buffer(dataTxBuffer, numBytesToTx), replyBootpEndpoint);

	std::cout << "\tSend BOOTP reply!" << std::endl;
}

bool EthernetComm::verifyBootpRequest(uint8_t* ptrData)
{
	ptrBootpRequestPacket = reinterpret_cast<BOOTP_PACKET*>(ptrData);

	//Validate the BOOTP_PACKET content
	if (ptrBootpRequestPacket->messageType != BootpProperties::BOOT_REQUEST)
	{
		return false;
	}

	if (ptrBootpRequestPacket->hardwareType != BootpProperties::HARDWARE_TYPE_ETHERNET)
	{
		return false;
	}

	if (ptrBootpRequestPacket->hardwareAddress != BootpProperties::SIZE_HW_ADDRESS)
	{
		return false;
	}

	if (ptrBootpRequestPacket->hops != BootpProperties::HOPS)
	{
		return false;
	}

	isHostName1 = false;
	if ((ptrBootpRequestPacket->serverHostName[0] == BootpProperties::hostName1[0]) &&
		(ptrBootpRequestPacket->serverHostName[1] == BootpProperties::hostName1[1]) &&
		(ptrBootpRequestPacket->serverHostName[2] == BootpProperties::hostName1[2]) &&
		(ptrBootpRequestPacket->serverHostName[3] == BootpProperties::hostName1[3]))
	{
		isHostName1 = true;
		return true;
	}
	else
	{
		if ((ptrBootpRequestPacket->serverHostName[0] == BootpProperties::hostName2[0]) &&
			(ptrBootpRequestPacket->serverHostName[1] == BootpProperties::hostName2[1]) &&
			(ptrBootpRequestPacket->serverHostName[2] == BootpProperties::hostName2[2]) &&
			(ptrBootpRequestPacket->serverHostName[3] == BootpProperties::hostName2[3]) &&
			(ptrBootpRequestPacket->serverHostName[4] == BootpProperties::hostName2[4]) &&
			(ptrBootpRequestPacket->serverHostName[5] == BootpProperties::hostName2[5]) &&
			(ptrBootpRequestPacket->serverHostName[6] == BootpProperties::hostName2[6]))
		{
			return true;
		}
		return false;
	}
}

void EthernetComm::prepareBootpReply(std::vector<uint8_t>* clientIpAddr,
	std::vector<uint8_t>* serverIpAddr)
{
	uint16_t i = 0;
	
	//Message type property is BOOTP Reply 
	dataTxBuffer.at(i++) = BootpProperties::BOOT_REPLY; 

	//Hardware type property is Ethernet
	dataTxBuffer.at(i++) = BootpProperties::HARDWARE_TYPE_ETHERNET;

	//Hardware address length property is 6 bytes
	dataTxBuffer.at(i++) = BootpProperties::SIZE_HW_ADDRESS;

	//Hops property is zero
	dataTxBuffer.at(i++) = BootpProperties::HOPS;

	//Copy the Transaction ID property from BOOTP request packet
	std::memcpy(&dataTxBuffer[i], 
		        &ptrBootpRequestPacket->transactionId[0], 
				BootpProperties::SIZE_TRANSACTION_ID);
	i += BootpProperties::SIZE_TRANSACTION_ID;

	//Copy the Second Elapsed property from BOOTP request packet
	std::memcpy(&dataTxBuffer[i], 
				&ptrBootpRequestPacket->secondElapsed[0], 
				BootpProperties::SIZE_SECONDS_ELAPSED);
	i += BootpProperties::SIZE_SECONDS_ELAPSED;

	//BOOTP flag address property is filled in with 0x00
	std::fill(&dataTxBuffer[i],
		      &dataTxBuffer[i + BootpProperties::SIZE_BOOTP_FLAG], 
		      BootpProperties::DEFAULT_BYTE);
	i += BootpProperties::SIZE_BOOTP_FLAG;

	//Client IP address property is filled in with 0x00
	std::fill(&dataTxBuffer[i], 
		      &dataTxBuffer[i + BootpProperties::SIZE_IP_ADDRESS], 
			  BootpProperties::DEFAULT_BYTE);
	i += BootpProperties::SIZE_IP_ADDRESS;

	//Your (client) IP address property is the local host IP address 
	std::memcpy(&dataTxBuffer[i], 
		        &clientIpAddr->at(0), 
		        BootpProperties::SIZE_IP_ADDRESS);
	i += BootpProperties::SIZE_IP_ADDRESS;
	
	//Next server IP address property is the address of BSL ethernet target
	std::memcpy(&dataTxBuffer[i], 
		        &serverIpAddr->at(0), 
				BootpProperties::SIZE_IP_ADDRESS);
	i += BootpProperties::SIZE_IP_ADDRESS;
	
	//Relay agent IP address property is filled in with 0x00
	std::fill(&dataTxBuffer[i], 
		      &dataTxBuffer[i + BootpProperties::SIZE_IP_ADDRESS],
		      BootpProperties::DEFAULT_BYTE);
	i += BootpProperties::SIZE_IP_ADDRESS;
	
	//Client MAC address property is copied from the BOOTP request packet MAC address
	std::memcpy(&dataTxBuffer[i], 
	   	        &ptrBootpRequestPacket->macAddress[0], 
		        BootpProperties::SIZE_HW_ADDRESS);
	i += BootpProperties::SIZE_HW_ADDRESS;
	
	//Client hardware address padding property is filled in with 0x00
	std::fill(&dataTxBuffer[i], 
		      &dataTxBuffer[i + BootpProperties::SIZE_MAC_ADDRESS_PADDING], 
		      BootpProperties::DEFAULT_BYTE);
	i += BootpProperties::SIZE_MAC_ADDRESS_PADDING;
	
	//Server host name property is copied from BOOTP request packet server host name
	uint8_t sizeHostNameSize = BootpProperties::SIZE_HOSTNAME_2;
	if (isHostName1)
	{
		sizeHostNameSize = BootpProperties::SIZE_HOSTNAME_1;
	}
	std::memcpy(&dataTxBuffer[i], 
		        &ptrBootpRequestPacket->serverHostName[0], 
		        sizeHostNameSize); 
	i += sizeHostNameSize;
	
	//The server host name property has to be padded with 0x00 for the total area is SIZE_SERVER_HOST_NAME (63 bytes) 
	uint16_t paddingSizeHostName = BootpProperties::SIZE_SERVER_HOST_NAME - sizeHostNameSize;
	std::fill(&dataTxBuffer[i],
		      &dataTxBuffer[i + paddingSizeHostName], 
		      BootpProperties::DEFAULT_BYTE); 
	i += paddingSizeHostName;
	
	//Boot file name property is using "firmware.bin"
	std::memcpy(&dataTxBuffer[i], 
		        &BootpProperties::firmwareDummy[0], 
		        BootpProperties::SIZE_FIRMWARE_DUMMY);
	i += BootpProperties::SIZE_FIRMWARE_DUMMY;
	
	//Boot file name property has to be padded with 0x00 for the total area is  SIZE_BOOTFILE_NAME (128 bytes)
	uint16_t paddingFirmwareName = BootpProperties::SIZE_BOOTFILE_NAME - BootpProperties::SIZE_FIRMWARE_DUMMY;
	std::fill(&dataTxBuffer[i], 
		      &dataTxBuffer[i + paddingFirmwareName], 
		      BootpProperties::DEFAULT_BYTE); 
	i += paddingFirmwareName;

	//BOOTP vendor property is using default value 0x00
	std::fill(&dataTxBuffer[i], 
		      &dataTxBuffer[i + BootpProperties::SIZE_VENDOR_NAME], 
		      BootpProperties::DEFAULT_BYTE); 
	i += BootpProperties::SIZE_VENDOR_NAME;

	numBytesToTx = i;
}

bool EthernetComm::receiveTftpReadRequest()
{
	bool receiveStatus = false;
	std::cout << "\tReady to receive the Tftp read request!" << std::endl;
	std::cout << "\tClient IP address: " << clientIpAddress << std::endl;

	tftpRequestEndpoint.address(boost::asio::ip::address_v4::from_string(clientIpAddress));

	socketTftpRequest.get_io_service().reset();
	socketTftpRequest.async_receive_from(boost::asio::buffer(dataRxBuffer), tftpRequestEndpoint,
		                                 boost::bind(&EthernetComm::onRead, this,
		                                 boost::asio::placeholders::error,
		                                 boost::asio::placeholders::bytes_transferred));
	timerEvent = false;
	timer.expires_from_now(boost::posix_time::seconds(TIMER_DEADLINE));
	timer.async_wait(boost::bind(&EthernetComm::onTimerSocketTftpRequest,
		             this,
		             boost::asio::placeholders::error));

	while (io_service.run_one(error))
	{
		if (!readError)
		{
			if (verifyTftpReadRequest(&dataRxBuffer[0]))
			{
				std::cout << "\tTFTP read request received!" << std::endl;
				repeatingWaitingRequest = 0;
				receiveStatus= true;
			}
			else
			{
				std::cout << "\t[ERROR_MESSAGE]There is invalid TFTP Read Request in the packet received!" << std::endl;
			}
			break;
		}

		if (timerEvent)
		{
			timerEvent = false;
			socketTftpRequest.cancel();
		}
	}
	io_service.run(error);
	io_service.reset();

	return receiveStatus;
}

bool EthernetComm::verifyTftpReadRequest(uint8_t *dataBuffer)
{
	TFTP_REQUEST_PACKET *ptr = reinterpret_cast<TFTP_REQUEST_PACKET*>(dataBuffer);

	//check the TFTP packet
	uint16_t receivedMessageType = ((ptr->messageType[0] << 8) |
		                             ptr->messageType[1]);
	if (receivedMessageType != TftpProperties::TFTP_RRQ)
	{
		return false;
	}

	for (uint8_t i = 0; i < BootpProperties::SIZE_FIRMWARE_DUMMY; i++)
	{
		if (ptr->firmwareFileName[i] != BootpProperties::firmwareDummy[i])
		{
			return false;
		}
	}

	for (uint8_t i = 0; i < TftpProperties::SIZE_TRANSFER_TYPE; i++)
	{
		if (ptr->transferType[i] != TftpProperties::octetTransferType[i])
		{
			return false;
		}
	}

	return true;
};

void EthernetComm::convertMacIpAddrStringToByte(std::string macAddrInput)
{
	boost::char_separator<char> sep(":-.\r");
	boost::tokenizer<boost::char_separator<char>> tokens(macAddrInput, sep);

	uint8_t i = 0;
	for (const std::string& t : tokens)
	{
		macAddress[i] = util->convertHexStringToByte(t);
		i++;
	}

	clientIpAddressInBytes.clear();
	boost::tokenizer<boost::char_separator<char>> tokensClientIpAddr(clientIpAddress, sep);
	for (const std::string& t : tokensClientIpAddr)
	{
		//The string input for IP already checked in the interpretMode in Interpreter class
		//   to be a valid ipV4 address
		uint8_t tmpToken = (uint8_t)(0x000000FF & (std::stoul(t, nullptr, 10)));
		clientIpAddressInBytes.push_back(tmpToken);
	}

	serverIpAddressInBytes.clear();
	boost::tokenizer<boost::char_separator<char>> tokensIpv4(ipv4Address, sep);
	for (const std::string& t : tokensIpv4)
	{
		//The string input for IP already checked in the interpretMode in Interpreter class
		//   to be a valid ipV4 address
		uint8_t tmpToken = (uint8_t)(0x000000FF & (std::stoul(t, nullptr, 10)));
		serverIpAddressInBytes.push_back(tmpToken);
	}
}

void EthernetComm::onRead(const boost::system::error_code& ec, size_t numBytes)
{
	readError = (ec || numBytes == 0);
	timer.cancel();
}

void EthernetComm::onTimerSocketBootp(const boost::system::error_code& ec)
{
	if (ec)
	{
		return;
	}
	//Timeout reached
	repeatingWaitingRequest++;
	timer.cancel();
	timerEvent = true;

	if (repeatingWaitingRequest == NUM_OF_TRY_WAITING_BOOTP_OR_TFTP)
	{
		socketBootp.cancel();
		throw std::runtime_error("[ERROR_MESSAGE]No BOOTP request received, exit the BSL-Scripter!");
	}
	else
	{
		std::cout << "\tWaiting for BOOTP request!" << std::endl;
	}
}

void EthernetComm::onTimerSocketTftpRequest(const boost::system::error_code& ec)
{
	if (ec)
	{
		return;
	}
	//Timeout reached
	repeatingWaitingRequest++;
	timer.cancel();
	timerEvent = true;

	if (repeatingWaitingRequest == NUM_OF_TRY_WAITING_BOOTP_OR_TFTP)
	{
		socketTftpRequest.cancel();
		throw std::runtime_error("[ERROR_MESSAGE]No TFTP read request received, exit the BSL-Scripter!");
	}
	else
	{
		std::cout << "\tWaiting for TFTP read request!" << std::endl;
		return;
	}
}

void EthernetComm::onTimerSocketTftpAck(const boost::system::error_code& ec)
{
	if (ec)
	{
		return;
	}
	//Timeout reached
	repeatingWaitingRequest++;
	timer.cancel();
	timerEvent = true;

	if (repeatingWaitingRequest == NUM_OF_TRY_WAITING_BOOTP_OR_TFTP)
	{
		socketTftpData.cancel();
		throw std::runtime_error("[ERROR_MESSAGE]No ACK from the TFTP data transmission!");
	}
	else
	{
		std::cout << "\tWaiting for ACK from the TFTP data transmission!" << std::endl;
		return;
	}
}


