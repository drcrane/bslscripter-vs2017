/*
==BslE4xxResponseHandler.h==

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

class BslE4xxResponseHandler
{
protected:

	/***********************************************************************************
	* BSL MSP432E4xx expected command bytes length
	***********************************************************************************/
	static const uint8_t SIZE_SYNC_UART_BYTES = 2;
	static const uint8_t SIZE_PING = 3;
	static const uint8_t SIZE_GET_STATUS = 3;
	static const uint8_t SIZE_ACK_RESPONSE = 2;

	/***********************************************************************************
	* BSL MSP432E4xx commands buffer
	***********************************************************************************/
	static const uint8_t SYNC_UART_BYTES[SIZE_SYNC_UART_BYTES];
	static const uint8_t GET_STATUS[SIZE_GET_STATUS];
	static const uint8_t ACK_HOST[SIZE_ACK_RESPONSE];
	static const uint8_t PING[SIZE_PING];

	/***********************************************************************************
	* BSL MSP432E4xx commands bytes
	***********************************************************************************/
	static const uint8_t PING_BYTE = 0x20;
	static const uint8_t DOWNLOAD_CMD = 0x21;
	static const uint8_t GET_STATUS_CMD = 0x23;
	static const uint8_t SEND_DATA_CMD = 0x24;

	/***********************************************************************************
	* BSL MSP432E4xx ACK response
	***********************************************************************************/
	static const uint8_t E4XX_SUCCESSFUL_OPERATION = 0x00;
	static const uint8_t E4XX_ACK = 0xCC;
	static const uint8_t E4XX_NACK = 0x33;

	/***********************************************************************************
	* BSL MSP432E4xx error codes
	***********************************************************************************/
	static const uint8_t E4XX_RET_SUCCESS = 0x40;
	static const uint8_t E4XX_RET_UNKNOWN_CMD = 0x41;
	static const uint8_t E4XX_RET_INVALID_CMD = 0x42;
	static const uint8_t E4XX_RET_INVALID_ADR = 0x43;
	static const uint8_t E4XX_RET_FLASH_FAIL = 0x44;
	static const uint8_t E4XX_RET_CRC_FAIL = 0x45;

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
	uint32_t cycle;		 //Number of how many times the transmission already
						 //  occurs for the reading/writing memory
	uint32_t bytesSent;	 //Number of bytes written to the device including 
	                     //  the additional byte(header byte, command byte,
	                     //  checksum byte)	in one cycle transmission
	std::string response;//Translation of response bytes into information in string

	std::vector<uint8_t> wrapTxBuffer;
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
	*Parameters:  CommandParams *cmdParams: command data struct 
	              std::vector<uint8_t>* buffer: pointer to rxBuffer
	*Returns:	  std::string
	***********************************************************************************/
	std::string translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer);

private:

};