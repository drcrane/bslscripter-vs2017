/*
==BslResponseHandler.h==

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

class BslResponseHandler
{
protected:

	/***********************************************************************************
	* ACK byte definition
	***********************************************************************************/
	static const uint8_t ACK_HEADER_INCORRECT = 0x51;
	static const uint8_t ACK_CHECKSUM_INCORRECT = 0x52;
	static const uint8_t ACK_PACKET_SIZE_ZERO = 0x53;
	static const uint8_t ACK_PACKET_SIZE_EXCEEDS_BUFFER = 0x54;
	static const uint8_t UNKNOWN_ERROR = 0x55;
	static const uint8_t UNKNOWN_BAUD_RATE = 0x56;

	/***********************************************************************************
	* BSL response message byte definition
	***********************************************************************************/
	static const uint8_t HEADER_BYTE = 0x80;
	static const uint8_t USB_HEADER_BYTE = 0x3F;
	static const uint8_t BSL_DATA_REPLY = 0x3A;
	static const uint8_t BSL_MESSAGE_REPLY = 0x3B;
	static const uint8_t FLASH_ACK = 0x00;

	/***********************************************************************************
	* BSL error codes
	***********************************************************************************/
	static const uint8_t SUCCESSFUL_OPERATION = 0x00;
	static const uint8_t NO_EXCEPTIONS = 0x00;
	static const uint8_t FLASH_WRITE_CHECK_FAILED = 0x01;
	static const uint8_t FLASH_FAIL_BIT_SET = 0x02;
	static const uint8_t VOLTAGE_CHANGE_DURING_PROGRAM = 0x03;
	static const uint8_t BSL_LOCKED = 0x04;
	static const uint8_t BSL_PASSWORD_ERROR = 0x05;
	static const uint8_t WRITE_FORBIDDEN = 0x06;
	static const uint8_t UNKNOWN_COMMAND = 0x07;
	
	/***********************************************************************************
	* Class variables
	***********************************************************************************/
	Util* util;
	ModeParams* modeParams;

	bool errorAck;
	bool errorStatus;
	uint8_t ack;
   	std::string errorAckMessage;
	std::string errorStatusMessage;

 	uint32_t bytesRead;  //Number of bytes read without 
	                     //   additional byte(header byte, command byte,
	                     //  checksum byte) 
	uint32_t bytesWrite; //Number of bytes written to the device including 
						 //  the additional byte(header byte, command byte,
	                     //  checksum byte)
	uint32_t cnt;        //Number of bytes read from the device including 
					     //  additional byte(header byte, command byte,
	                     //  checksum byte)
	uint32_t pureData;	 //Number of bytes read/written from/to the device 
	                     //  exclusively from additional byte
	uint32_t cycle;		 //Number of how many times the transmission already
	                     //  occurs for the reading/writing memory
	uint32_t bytesSent;

	std::vector<uint8_t> wrapTxBuffer;

	std::string response;
	bool crcInputAvailable;
	bool crcCheckResult;
	uint32_t crcInput;
	uint32_t concateBslVersionVendor;
	uint32_t concateBslVersionCI;
	uint32_t concateBslVersionAPI;
	uint32_t concateBslVersionPI;
	uint32_t concateBslBuildId;
	uint32_t concateCrcCheck;
	uint32_t txBufferSize;
	std::string bslVersionVendor;
	std::string bslVersionCI;
	std::string bslVersionAPI;
	std::string bslVersionPI;
	std::string bslBuildId;
	bool toggleInfoStatus;

	/***********************************************************************************
	*Function:    processResponse
	*Description: process and transalate the response bytes
	*Parameters:  uint8_t ack: ACK byte from BSL response
	              std::vector<uint8_t> *response: additional received response bytes
	*Returns:	  -
	***********************************************************************************/
	void processResponse(uint8_t ack, std::vector<uint8_t> *response);

	/***********************************************************************************
	*Function:    retErrorAck
	*Description: return the status of error ACK
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	bool retErrorAck();

	/***********************************************************************************
	*Function:    retErrorStatus
	*Description: return the status of error message ACK
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	bool retErrorStatus();

	/***********************************************************************************
	*Function:    retErrorAckMessage
	*Description: return the error status message for ACK error 
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	std::string retErrorAckMessage();

	/***********************************************************************************
	*Function:    retErrorStatusMessage
	*Description: return the error status message for BSL error message 
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	std::string retErrorStatusMessage();

	/***********************************************************************************
	*Function:    retCrcCheckResult
	*Description: return status if the Crc comparison 
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	bool retCrcCheckResult();

	/***********************************************************************************
	*Function:    translateResponse
	*Description: return BSL response in message format
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	std::string translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    initToggleStatus
	*Description: initialize toggle status to false in the beginning of the program
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void initToggleStatus();

private	:

	/***********************************************************************************
	*Function:    jtagLockSignatureResponse
	*Description: translate the jtagLockSignature(CLI) bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message. 
				    It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void jtagLockSignatureResponse(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    jtagLockPasswordResponse
	*Description: translate the jtagLockPassword(CLI) bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message.
	                It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void jtagLockPasswordResponse(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    rxPasswordResponse
	*Description: translate the RX_PASSWORD and RX_PASSWORD_32 bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message.
	                It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void rxPasswordResponse(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    txBslVersionResponse
	*Description: translate the TX_BSL_VERSION bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message.
	                It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void txBslVersionResponse(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    txBslVersion32Response
	*Description: translate the TX_BSL_VERSION_32 bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message.
	                It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void txBslVersion32Response(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    crcCheckResponse
	*Description: translate the CRC_CHECK and CRC_CHECK_32 bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message.
	                It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void crcCheckResponse(std::vector<uint8_t>* buffer, uint8_t idx, bool crcInputAvailable);

	/***********************************************************************************
	*Function:    eraseSegmentResponse
	*Description: translate the ERASE_SEGMENT and ERASE_SEGMENT_32 bytes response 
	                to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	uint8_t idx : start of bytes that contains message.
	It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void eraseSegmentResponse(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    massEraseResponse
	*Description: translate the MASS_ERASE bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message.
	                It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void massEraseResponse(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    toggleInfoResponse
	*Description: translate the TOGGLE_INFO bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	uint8_t idx : start of bytes that contains message.
	It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void toggleInfoResponse(std::vector<uint8_t>* buffer, uint8_t idx);

	/***********************************************************************************
	*Function:    txBufferSizeResponse
	*Description: translate the TX_BUFFER_SIZE bytes response to string
	*Parameters:  std::vector<uint8_t>* buffer: received buffer from transmission
	              uint8_t idx : start of bytes that contains message.
	                It is different between serial and USB protocol
	*Returns:	  -
	***********************************************************************************/
	void txBufferSizeResponse(std::vector<uint8_t>* buffer, uint8_t idx);
};