/*
==BslE4xxResponseHandler.cpp==

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
			
const uint8_t BslE4xxResponseHandler::SYNC_UART_BYTES[SIZE_SYNC_UART_BYTES] = { 0x55, 0x55 };
const uint8_t BslE4xxResponseHandler::PING[SIZE_PING] = { SIZE_PING, PING_BYTE, PING_BYTE };
const uint8_t BslE4xxResponseHandler::GET_STATUS[SIZE_GET_STATUS] = { SIZE_GET_STATUS, GET_STATUS_CMD, GET_STATUS_CMD };
const uint8_t BslE4xxResponseHandler::ACK_HOST[SIZE_ACK_RESPONSE] = { E4XX_SUCCESSFUL_OPERATION, E4XX_ACK };

void BslE4xxResponseHandler::processResponse(uint8_t ack, std::vector<uint8_t> *response)
{
	errorAck = true;
	errorAckMessage = "";
	errorStatus = true;
	errorStatusMessage = "";

	switch (ack)
	{
	case(E4XX_ACK) :
		errorAck = false;
		break;
	case(E4XX_NACK) :
		errorAckMessage = ("[ACK_ERROR_MESSAGE]BSL responses with NACK!");
		break;
	default:
		errorAckMessage = ("[ACK_ERROR_MESSAGE]Unknown ACK value!");
		break;
	}
		
	if (response->at(4) == SIZE_GET_STATUS)
	{
		switch (response->at(5))
		{
		case(E4XX_RET_SUCCESS) :
			errorStatus = false;
			break;
		case(E4XX_RET_UNKNOWN_CMD) :
			errorStatusMessage = "[ERROR_MESSAGE]Unknown command!";
			break;
		case(E4XX_RET_INVALID_CMD) :
			errorStatusMessage = "[ERROR_MESSAGE]Invalid command!";
			break;
		case(E4XX_RET_INVALID_ADR) :
			errorStatusMessage = "[ERROR_MESSAGE]Invalid address!";
			break;
		case(E4XX_RET_FLASH_FAIL) :
			errorStatusMessage = "[ERROR_MESSAGE]Flash programming is failed!";
			break;
		case(E4XX_RET_CRC_FAIL) :
			errorStatusMessage = "[ERROR_MESSAGE]CRC calculation is not matched!";
			break;
		default:
			break;
		}
	}
	else
	{
		errorStatus = false;
	}
}

bool BslE4xxResponseHandler::retErrorAck()
{
	return errorAck;
}

bool BslE4xxResponseHandler::retErrorStatus()
{
	return errorStatus;
}

std::string BslE4xxResponseHandler::retErrorAckMessage()
{
	return errorAckMessage;
}

std::string BslE4xxResponseHandler::retErrorStatusMessage()
{
	return errorStatusMessage;
}


std::string BslE4xxResponseHandler::translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer)
{
	response = "";
	
	switch (cmdParams->command)
	{
	case(Command::SET_PC_32) :
		if ((buffer->at(0) == E4XX_SUCCESSFUL_OPERATION) &&
			(buffer->at(1) == E4XX_ACK))
		{
			response = "[SUCCESS_MESSAGE]SET PC to " + cmdParams->params.at(0) + " is executed!";
		}
		else
		{
			response = "[ERROR_MESSAGE]SET PC  to " + cmdParams->params.at(1) +" is failed to be executed!";
		}
		break;

	case(Command::REBOOT_RESET) :
		if ((buffer->at(0) == E4XX_SUCCESSFUL_OPERATION) &&
			(buffer->at(1) == E4XX_ACK))
		{
			response = "[SUCCESS_MESSAGE]Reboot reset is executed!";
		}
		else
		{
			response = "[ERROR_MESSAGE]Reboot reset is failed to be executed!";
		}
		break;
 
	default:
		break;
	}
	return response;
}
