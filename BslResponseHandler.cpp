/*
==BslResponseHandler.cpp==

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

void BslResponseHandler::processResponse(uint8_t ack, std::vector<uint8_t> *response)
{
	errorAck = true;
	errorAckMessage = "";

	if (modeParams->protocol != Protocol::Usb)
	{
		switch (ack)
		{
			case(SUCCESSFUL_OPERATION) :
				errorAck = false;
				break;
			case(ACK_HEADER_INCORRECT) :
				errorAckMessage = ("[ACK_ERROR_MESSAGE]Header incorrect!");
				break;
			case(ACK_CHECKSUM_INCORRECT) :
				errorAckMessage = ("[ACK_ERROR_MESSAGE]Checksum incorrect!");
				break;
			case(ACK_PACKET_SIZE_ZERO) :
				errorAckMessage = ("[ACK_ERROR_MESSAGE]Packet size is Zero!");
				break;
			case(ACK_PACKET_SIZE_EXCEEDS_BUFFER) :
				errorAckMessage = ("[ACK_ERROR_MESSAGE]Packet Size exceeds maximum buffer size!");
				break;
			case(UNKNOWN_ERROR) :
				errorAckMessage = ("[ACK_ERROR_MESSAGE]Unknown Error!");
				break;
			case(UNKNOWN_BAUD_RATE) :
				errorAckMessage = ("[ACK_ERROR_MESSAGE]Unknown Baud Rate!");
				break;
			default:
				errorAckMessage = ("[ACK_ERROR_MESSAGE]Unknown ACK value!");
				break;
		}
	}
	else
	{
		if (response->at(0) == USB_HEADER_BYTE)
		{
			errorAck = false;
		}
	}

	errorStatusMessage = "";
	errorStatus = true;

	uint8_t posByte = 3;
	if (modeParams->protocol == Protocol::Usb)
	{
		posByte = 2;
	}
	if (response->at(posByte) == BSL_MESSAGE_REPLY)
	{
		switch (response->at(posByte + 1))
		{
			case(SUCCESSFUL_OPERATION) :
				errorStatus = false;
				break;
			case(FLASH_WRITE_CHECK_FAILED) :
				errorStatusMessage = "[ERROR_MESSAGE]Flash write check failed!";
				break;
			case(FLASH_FAIL_BIT_SET) :
				errorStatusMessage = "[ERROR_MESSAGE]Flash fail bit set!";
				break;
			case(VOLTAGE_CHANGE_DURING_PROGRAM) :
				errorStatusMessage = "[ERROR_MESSAGE]Voltage changed during Programming!";
				break;
			case(BSL_LOCKED) :
				errorStatusMessage = "[ERROR_MESSAGE]BSL is locked!";
				break;
			case(BSL_PASSWORD_ERROR) :
				errorStatusMessage = "[ERROR_MESSAGE]BSL Password is error!";
				break;
			case(WRITE_FORBIDDEN) :
				errorStatusMessage = "[ERROR_MESSAGE]Writing is forbidden!";
				break;
			case(UNKNOWN_COMMAND) :
				errorStatusMessage = "[ERROR_MESSAGE]Unknown Command!";
				break;
		}
	}
	else if (response->at(posByte) == BSL_DATA_REPLY)
	{
		errorStatusMessage = "";
		errorStatus = false;
		return;
	}
}

bool BslResponseHandler::retErrorAck()
{
	return errorAck;
}
  
bool BslResponseHandler::retErrorStatus()
{
	return errorStatus;
}
 
std::string BslResponseHandler::retErrorAckMessage()
{
	return errorAckMessage;
}

std::string BslResponseHandler::retErrorStatusMessage()
{
	return errorStatusMessage;
}


std::string BslResponseHandler::translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx)
{
	response = "";
	switch (cmdParams->command)
	{

	case(Command::JTAG_LOCK) :
		jtagLockSignatureResponse(buffer, idx);
		break;

	case(Command::JTAG_PASSWORD) :
		jtagLockPasswordResponse(buffer, idx);
		break;

	case(Command::RX_PASSWORD) :
		rxPasswordResponse(buffer, idx);
		break;

	case(Command::RX_PASSWORD_32) :
		rxPasswordResponse(buffer, idx);
		break;

	case(Command::TX_BSL_VERSION) :
		txBslVersionResponse(buffer, idx);
		break;

	case(Command::TX_BSL_VERSION_32) :
		txBslVersion32Response(buffer, idx);
		break;

	case(Command::CRC_CHECK) :
		crcInputAvailable = false;
		if (cmdParams->params.size() == 3)
		{
			crcInputAvailable = true;
			crcInput = util->convertHexStringToUlong(cmdParams->params.at(2));
		}
		crcCheckResponse(buffer, idx, crcInputAvailable);
		break;

	case(Command::CRC_CHECK_32) :
		crcInputAvailable = false;
		if (cmdParams->params.size() == 3)
		{
			crcInputAvailable = true;
			crcInput = util->convertHexStringToUlong(cmdParams->params.at(2));
		}
		crcCheckResponse(buffer, idx, crcInputAvailable);
		break;

	case(Command::ERASE_SEGMENT) :
		eraseSegmentResponse(buffer, idx);
		break;

	case(Command::ERASE_SEGMENT_32) :
		eraseSegmentResponse(buffer, idx);
		break;

	case(Command::MASS_ERASE) :
		massEraseResponse(buffer, idx);
		break;

	case(Command::TOGGLE_INFO) :
		toggleInfoResponse(buffer, idx);
		break;

	case(Command::TX_BUFFER_SIZE) :
		txBufferSizeResponse(buffer, idx);
		break;

	default:
		return "";
		break;
	}
	return response;
}

void BslResponseHandler::jtagLockSignatureResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	if (buffer->at(idx) == 0x00)
	{
		response = "[SUCCESS_MESSAGE]JTAG Signature is written!";
	}
}

void BslResponseHandler::jtagLockPasswordResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	if (buffer->at(idx) == 0x00)
	{
		response = "[SUCCESS_MESSAGE]JTAG Password is written!";
	}
}

void BslResponseHandler::rxPasswordResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	if (buffer->at(idx) == 0x00)
	{
		response = "[SUCCESS_MESSAGE]BSL Password is correct!";
	}
}

void BslResponseHandler::txBslVersionResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	if (buffer->at(idx) == 0x00)
	{
		bslVersionVendor = "TI";
	}
	else
	{
		bslVersionVendor = util->fillInZero(util->convertByteToString(buffer->at(idx), false), 2);
	}
	bslVersionCI = util->fillInZero(util->convertByteToString(buffer->at(idx + 1), false), 2);
	bslVersionAPI = util->fillInZero(util->convertByteToString(buffer->at(idx + 2), false), 2);
	bslVersionPI = util->fillInZero(util->convertByteToString(buffer->at(idx + 3), false), 2);
	response = ("[SUCCESS_MESSAGE]Vendor:[" + bslVersionVendor +
				"] CI:[" + bslVersionCI +
				"] API:[" + bslVersionAPI +
				"] PI:[" + bslVersionPI + "]");
}

void BslResponseHandler::txBslVersion32Response(std::vector<uint8_t>* buffer, uint8_t idx)
{
	concateBslVersionVendor = buffer->at(idx + 1) + (((uint32_t)buffer->at(idx)) << 8);
	concateBslVersionCI = buffer->at(idx + 3) + (((uint32_t)buffer->at(idx + 2)) << 8);
	concateBslVersionAPI = buffer->at(idx + 5) + (((uint32_t)buffer->at(idx + 4)) << 8);
	concateBslVersionPI = buffer->at(idx + 7) + (((uint32_t)buffer->at(idx + 6)) << 8);
	concateBslBuildId = buffer->at(idx + 9) + (((uint32_t)buffer->at(idx + 8)) << 8);
	if (concateBslVersionVendor == 0x00)
	{
		bslVersionVendor = "TI";
	}
	else
	{
		bslVersionVendor = util->fillInZero(util->convertUlongToString(concateBslVersionVendor, false), 4);
	}
	bslVersionCI = util->fillInZero(util->convertUlongToString(concateBslVersionCI, false), 4);
	bslVersionAPI = util->fillInZero(util->convertUlongToString(concateBslVersionAPI, false), 4);
	bslVersionPI = util->fillInZero(util->convertUlongToString(concateBslVersionPI, false), 4);
	bslBuildId = util->fillInZero(util->convertUlongToString(concateBslBuildId, false), 4);
	response = ("[SUCCESS_MESSAGE]Vendor:[" + bslVersionVendor +
				"] CI:[" + bslVersionCI +
				"] API:[" + bslVersionAPI +
				"] PI:[" + bslVersionPI + +
				"] Build-ID:[" + bslBuildId + "]");
}

void BslResponseHandler::crcCheckResponse(std::vector<uint8_t>* buffer, uint8_t idx, bool crcInputAvailable)
{
	concateCrcCheck = buffer->at(idx) + (((uint32_t)buffer->at(idx + 1)) << 8);
	if (crcInputAvailable)
	{
		if (util->compareInt(concateCrcCheck, crcInput))
		{
			crcCheckResult = true;
			response = "[SUCCESS_MESSAGE]CRC check match!";
		}
		else
		{
			crcCheckResult = false;
			response =  ("[ERROR_MESSAGE]CRC check does not match; given: " +
				         util->convertUlongToString(crcInput, true) +
						 " calculated from BSL: " +
				         util->convertUlongToString(concateCrcCheck, true));
		}
	}
	else
	{
		response = ("[SUCCESS_MESSAGE]CRC calculated from BSL: " + 
			        util->convertUlongToString(concateCrcCheck, true));
	}
}

void BslResponseHandler::eraseSegmentResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	if (buffer->at(idx) == SUCCESSFUL_OPERATION)
	{
		response = "[SUCCESS_MESSAGE]Erase Segment is successful!";
	}
	else
	{
		response = "[ERROR_MESSAGE]Erase Segment is failed!";
	}
}

void BslResponseHandler::massEraseResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	if (buffer->at(idx) == SUCCESSFUL_OPERATION)
	{
		response = "[SUCCESS_MESSAGE]Mass Erase is successful!";
	}
	else
	{
		response = "[ERROR_MESSAGE]Mass Erase is failed!";
	}
}

void BslResponseHandler::toggleInfoResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	if (buffer->at(idx) == SUCCESSFUL_OPERATION)
	{
		if (toggleInfoStatus)
		{
			response = "[SUCCESS_MESSAGE]Info Memory is now unlocked!";
			toggleInfoStatus = false;
		}
		else
		{
			response = "[SUCCESS_MESSAGE]Info Memory is now locked!";
			toggleInfoStatus = true;
		}
	}
}

void BslResponseHandler::txBufferSizeResponse(std::vector<uint8_t>* buffer, uint8_t idx)
{
	txBufferSize = buffer->at(idx) + (((uint32_t)buffer->at(idx + 1)) << 8);
}

bool  BslResponseHandler::retCrcCheckResult()
{
	return crcCheckResult;
}

void  BslResponseHandler::initToggleStatus()
{
	toggleInfoStatus = true;
}
