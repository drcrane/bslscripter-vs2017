/*
==UsbComm.cpp==

Copyright (c) 2015-2017 Texas Instruments Incorporated

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

UsbComm::UsbComm(ModeParams* modeParams, Util *util)
	: handle(nullptr) ,
	  rxUsbBuffer(67100)
{	
	BslResponseHandler::util = util;
	BslResponseHandler::modeParams = modeParams;
	BslResponseHandler::initToggleStatus();
	this->util = util;
}

UsbComm::~UsbComm()
{	

}

std::vector<uint8_t>* UsbComm::retPtrTxBuffer()
{
	return &wrapTxBuffer;
}

std::vector<uint8_t>* UsbComm::retPtrRxBuffer()
{
	return &rxUsbBuffer;
}

void UsbComm::init(ModeParams* modeParams)
{
	if (hid_init())
	{
		throw std::runtime_error("HID init failed");
	}

	UsbComm::handle = hid_open(VID, PID, nullptr);

	if (!handle)
	{
		throw std::runtime_error("Unable to open the HID device");
	}

}


void UsbComm::transmitBuffer(std::vector<uint8_t>* txBuffer)
{
	wrapTxBuffer.clear();
	wrapTxBuffer.push_back(0x3F);
	wrapTxBuffer.push_back(txBuffer->size());

	for (uint32_t i = 0; i < txBuffer->size(); i++)
	{
		wrapTxBuffer.push_back(txBuffer->at(i));
	}

	int32_t res = hid_write(handle, &wrapTxBuffer.at(0), wrapTxBuffer.size());

	if (res < 0)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Data is not completely transmitted!");
	}

	bytesWrite += (txBuffer->size() - 4);
	bytesSent += (txBuffer->size() - 4);
}

void UsbComm::clearTempBuffer()
{
	for (uint8_t i = 0; i < 64; i++)
	{
		tempBuffer[i] = 0x00;
	}
}

std::vector<uint8_t>* UsbComm::receiveBuffer(int32_t size)
{
	if (size > 0)
	{
		int32_t res = hid_read_timeout(handle, &rxUsbBuffer[cnt], (size + 3), MS_DELAY_RX);
		if (res < 0)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Data is not completely received!");
		}
		pureData += size;
		cnt += (size + 3);
	}
	return &rxUsbBuffer;
}
  
void UsbComm::changeBaudRate(uint32_t baudRate)
{
	throw std::runtime_error("[ERROR_MESSAGE]Change Baud Rate is not applicable for USB communication!");
}

void UsbComm::close()
{
	hid_close(handle);
}

void UsbComm::resetBytesWritten()
{
	bytesWrite = 0;
}

uint32_t UsbComm::retBytesWritten()
{
	return bytesWrite;
}


void UsbComm::resetBytesSent()
{
	bytesSent = 0;
}

uint32_t UsbComm::retBytesSent()
{
	return bytesSent;
}

uint32_t UsbComm::retBytesRead()
{
	return pureData;
}

uint8_t UsbComm::retAck()
{
	return ack;
}

uint32_t UsbComm::retLastRxBufferIdx()
{
	return cnt;
}

void UsbComm::resetIdxAcc()
{
	pureData = 0;
	cnt = 0;
}


void UsbComm::processResponse(uint8_t ack, std::vector<uint8_t> *response)
{
	BslResponseHandler::processResponse(ack, response);
}


bool UsbComm::retErrorAck()
{
	return BslResponseHandler::retErrorAck();
}

bool UsbComm::retErrorStatus()
{
	return BslResponseHandler::retErrorStatus();
}

std::string UsbComm::retErrorAckMessage()
{
	return BslResponseHandler::retErrorAckMessage();
}

std::string UsbComm::retErrorStatusMessage()
{
	return BslResponseHandler::retErrorStatusMessage();
}

bool UsbComm::retCrcCheckResult()
{
	return BslResponseHandler::retCrcCheckResult();
}

std::string UsbComm::translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx)
{
	return BslResponseHandler::translateResponse(cmdParams, buffer, idx);
}
