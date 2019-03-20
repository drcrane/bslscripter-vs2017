/*
==EthernetComm.h==

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

#pragma once

#include "stdafx.h"

class EthernetComm : public CommInterface, public BslE4xxEthernetResponseHandler
{
public:

	EthernetComm(ModeParams* modeParams, Util *util);
	~EthernetComm();
  
	/***********************************************************************************
	*Function:    init
	*Description: do the initialization of the communication protocol based
	*             on the parameters of the given command
	*Parameters:  ModeParams modeParams: parameters from interpreter
	*Returns:	  -
	***********************************************************************************/
	void init(ModeParams* modeParams) OVERRIDE;

	/***********************************************************************************
	*Function:    transmitBuffer
	*Description: send the BSL command
	*Parameters:  std::vector<uint8_t>* txBuffer: prepared buffer from Target class
	*Returns:	  -
	***********************************************************************************/
	void transmitBuffer(std::vector<uint8_t>* txBuffer) OVERRIDE;

	/***********************************************************************************
	*Function:	  receiveBuffer
	*Description:
	*Parameters:  int32_t size : number of bytes that should be received by the
	*                            scripter
	*                            in E4xx family, the number of bytes of the response 
	*                            always fix and comes from the Target class
	*Returns:     std::vector<uint8_t>*
	***********************************************************************************/
	std::vector<uint8_t>*  receiveBuffer(int32_t size) OVERRIDE;

	/***********************************************************************************
	*Function:    changeBaudRate
	*Description: change baud rate with the given baudRate on the serial communication
	*Parameters:  uint32_t baudRate: new baudRate
	*Returns:	  -
	***********************************************************************************/
	void changeBaudRate(uint32_t baudRate) OVERRIDE;

	/***********************************************************************************
	*Function:	  close
	*Description: close communication port
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void close() OVERRIDE;

	/***********************************************************************************
	*Function:	  resetBytesWritten
	*Description: bytesWritten is the variable which is used to accumulate the whole
	*             bytes written or programmed to the device for one firmware image.
	*             This number will be used to calculate the programming time.
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void resetBytesWritten() OVERRIDE;

	/***********************************************************************************
	*Function:	  retBytesWritten
	*Description: return the number of bytes written to the device specific from
	*             one firmware image source
	*Parameters:  -
	*Returns:     uint32_t
	***********************************************************************************/
	uint32_t retBytesWritten() OVERRIDE;

 	/***********************************************************************************
	*Function:	  resetBytesSent
	*Description: reset the bytesSent variable
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void resetBytesSent() OVERRIDE;

	/***********************************************************************************
	*Function:	  retBytesSent
	*Description: in E4xx family, the transmission is executed continuously for one big
	*             dataBlockRead refering to program one specified segment. bytesSent is
	*             the variable which accumulate how many bytes data has been transfered
	*             for the specific dataBlockRead.
	*Parameters:  -
	*Returns:     uint32_t
	***********************************************************************************/
	uint32_t retBytesSent() OVERRIDE;

	/***********************************************************************************
	*Function:	  retBytesRead
	*Description:
	*Parameters:  -
	*Returns:     uint32_t
	***********************************************************************************/
	uint32_t retBytesRead() OVERRIDE;

	/***********************************************************************************
	*Function:	  retAck
	*Description: return the ack from the BSL response
	*Parameters:  -
	*Returns:     uint8_t
	***********************************************************************************/
	uint8_t retAck() OVERRIDE;

	/***********************************************************************************
	*Function:	  retLastRxBufferIdx
	*Description: return the last index of dataRxBuffer which contains the BSL response
	*Parameters:  -
	*Returns:	  uint32_t
	***********************************************************************************/
	uint32_t retLastRxBufferIdx() OVERRIDE;

	/***********************************************************************************
	*Function:	  retPtrRxBuffer
	*Description: return the pointer to the dataRxBuffer
	*Parameters:  -
	*Returns:	  std::vector<uint8_t>*
	***********************************************************************************/
	std::vector<uint8_t>* retPtrRxBuffer() OVERRIDE;

	/***********************************************************************************
	*Function:	  retPtrTxBuffer
	*Description: return the pointer to the dataTxBuffer
	*Parameters:  -
	*Returns:	  std::vector<uint8_t>*
	***********************************************************************************/
	std::vector<uint8_t>* retPtrTxBuffer() OVERRIDE;

	/***********************************************************************************
	*Function:	  resetIdxAcc
	*Description: reset index calculation for the rxBuffer for reading memory process
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void resetIdxAcc() OVERRIDE;

	/***********************************************************************************
	*Function:	  processResponse
	*Description: calling processResponse from the inhereted class BslResponseHandler
	*Parameters:  uint8_t ack: received ACK byte
	*             std::vector<uint8_t> *response: pointer to rxBuffer
	*Returns:	  -
	***********************************************************************************/
	void processResponse(uint8_t ack, std::vector<uint8_t> *response) OVERRIDE;

	/***********************************************************************************
	*Function:	  retErrorAck
	*Description: calling retErrorAck from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	bool retErrorAck() OVERRIDE;

	/***********************************************************************************
	*Function:	  retErrorStatus
	*Description: calling retErrorStatus from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	bool retErrorStatus() OVERRIDE;

	/***********************************************************************************
	*Function:	  retErrorAckMessage
	*Description: calling retErrorAckMessage from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	std::string retErrorAckMessage() OVERRIDE;

	/***********************************************************************************
	*Function:	  retErrorStatusMessage
	*Description: calling retErrorStatusMessage from the inhereted class
	*             BslE4xxEthernetResponseHandler
	*Parameters:  --
	*Returns:	  std::string
	***********************************************************************************/
	std::string retErrorStatusMessage() OVERRIDE;

	/***********************************************************************************
	*Function:	  retCrcCheckResult
	*Description: calling retCrcCheckResult from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	bool retCrcCheckResult()OVERRIDE;

	/***********************************************************************************
	*Function:	  translateResponse
	*Description: calling translateResponse from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	std::string translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx) OVERRIDE;

private:

	Util *util;
	ModeParams *modeParams;

	/***********************************************************************************
	* PORT definition
	***********************************************************************************/
	static const uint8_t PORT_WOL_MAGIC_PACKET = 9;
	static const uint8_t BOOTP_SERVER_PORT = 67;
	static const uint8_t BOOTP_CLIENT_PORT = 68;
	static const uint8_t TFTP_PORT = 69;
	static const uint8_t TFTP_PORT_AUTOMATIC = 0;


	/***********************************************************************************
	* Other macros
	***********************************************************************************/
	static const uint8_t DEFAULT_BYTE;
	static const uint16_t MAX_SIZE_SOCKET_BUFFER = 1024;
	static const uint16_t MAX_SIZE_TFTP_DATA_BUFFER = 512;

	/***********************************************************************************
	* Socket
	***********************************************************************************/
	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket socketWol;
	boost::asio::ip::udp::socket socketBootp;
	boost::asio::ip::udp::socket socketTftpRequest;
	boost::asio::ip::udp::socket socketTftpData;

	/***********************************************************************************
	* Endpoint
	***********************************************************************************/
	boost::asio::ip::udp::endpoint replyBootpEndpoint;
	boost::asio::ip::udp::endpoint tftpRequestEndpoint;
	boost::asio::ip::udp::endpoint tftpSendpDataEndpoint;
	boost::asio::ip::udp::endpoint tftpRecpDataEndpoint;
	uint16_t tftpSendpDataEndpointPort;	

	bool timerEvent;
	bool readError;
	uint8_t repeatingWaitingRequest;
	static const uint8_t NUM_OF_TRY_WAITING_BOOTP_OR_TFTP;
	static const uint8_t TIMER_DEADLINE; 
	boost::asio::deadline_timer timer;

	/***********************************************************************************
	* Boost error code
	***********************************************************************************/
	boost::system::error_code error;

	/***********************************************************************************
	* Class variables
	***********************************************************************************/
	uint8_t macAddress[BootpProperties::SIZE_HW_ADDRESS];
	std::string clientIpAddress;
	std::vector<std::string> ipAddressProperties;
	std::string ipv4Address;
	std::vector<uint8_t> clientIpAddressInBytes;
	std::vector<uint8_t> serverIpAddressInBytes;
 	std::vector<uint8_t> wolMagicPacket;
	std::vector<uint8_t> dataRxBuffer;
	std::vector<uint8_t> dataTxBuffer;
	uint32_t numBytesToTx;

	BOOTP_PACKET *ptrBootpRequestPacket;
 	bool isHostName1;

	/***********************************************************************************
	*Function:	  convertMacIpAddrStringToByte
	*Description: convert the macAddress and client and server ipv4address into bytes
	*Parameters:  std::string macAddrInput
	*Returns:	  std::string
	***********************************************************************************/
	void convertMacIpAddrStringToByte(std::string macAddrInput);

	/***********************************************************************************
	*Function:	  prepareWolMagicPacket
	*Description: the wake-on-LAN magic packet contains of 4 repetition of MAC address
	*             and store it in wolMagicPacket vector
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void prepareWolMagicPacket();

	/***********************************************************************************
	*Function:	  sendWolMagicPacket
	*Description: send the wolMagicPacket with broadcast mode
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void sendWolMagicPacket();

	/***********************************************************************************
	*Function:	  receiveBootpRequest
	*Description: receive BootpRequest from client
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	bool receiveBootpRequest();

	/***********************************************************************************
	*Function:	  verifyBootpRequest
	*Description: verify if the Bootp request is valid
	*Parameters:  uint8_t* dataBuffer: pointer to the dataRxBuffer
	*Returns:	  -
	***********************************************************************************/
	bool verifyBootpRequest(uint8_t* dataBuffer);

	/***********************************************************************************
	*Function:	  prepareBootpReply
	*Description: prepare the Bootp reply package based on the received Bootp request
	*Parameters:  std::vector<uint8_t>* clientIpAddr: pointer the clientIpAddr in bytes
	*             std::vector<uint8_t>* serverIpAddr: pointer to serverIpAddr in bytes
	*             std::vector<uint8_t>* serverHostName: pointer to the server host name 
	*Returns:	  -
	***********************************************************************************/
	void prepareBootpReply(std::vector<uint8_t>* clientIpAddr, std::vector<uint8_t>* serverIpAddr);
	
	/***********************************************************************************
	*Function:	  sendBootpReply
	*Description: send the Bootp reply
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void sendBootpReply();

	/***********************************************************************************
	*Function:	  receive the Tftp read request from client
	*Description: send the Bootp reply
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	bool receiveTftpReadRequest();

	/***********************************************************************************
	*Function:	  verifyTftpReadRequest
	*Description: verify if theTftp read request from client is valid
	*Parameters:  pointer to dataRxBuffer
	*Returns:	  bool
	***********************************************************************************/
	bool verifyTftpReadRequest(uint8_t *dataBuffer);

	/***********************************************************************************
	*Function:	  onRead
	*Description: bind-function in receiving data in the socket that will be called
	*             once the receive process is done
	*Parameters:  const boost::system::error_code& ec:  error code from reading process
	*             size_t numBytes: number of bytes received in the socket
	*Returns:	  -
	***********************************************************************************/
	void onRead(const boost::system::error_code& ec, size_t numBytes);

	/***********************************************************************************
	*Function:	  onTimerSocketBootp
	*Description: bind-function for timer in waiting the BOOTP request
	*Parameters:  const boost::system::error_code& ec:  error code from timer execution
	*Returns:	  -
	***********************************************************************************/
	void onTimerSocketBootp(const boost::system::error_code& ec);

	/***********************************************************************************
	*Function:	  onTimerSocketTftpRequest
	*Description: bind-function for timer in waiting the TFTP request
	*Parameters:  const boost::system::error_code& ec:  error code from timer execution
	*Returns:	  -
	***********************************************************************************/
	void onTimerSocketTftpRequest(const boost::system::error_code& ec);

	/***********************************************************************************
	*Function:	  onTimerSocketTftpAck
	*Description: bind-function for timer in waiting the TFTP ACK data transmission
	*Parameters:  const boost::system::error_code& ec:  error code from timer execution
	*Returns:	  -
	***********************************************************************************/
	void onTimerSocketTftpAck(const boost::system::error_code& ec);
};