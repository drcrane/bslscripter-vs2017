/*
==Family16.cpp==

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
 
const uint8_t Family16bit::CMD_RX_DATA_BLOCK_FAST;
const uint8_t Family16bit::CMD_RX_PASSWORD;
const uint8_t Family16bit::CMD_TOGGLE_INFO;
const uint8_t Family16bit::CMD_TX_BSL_VERSION;
const uint8_t Family16bit::CMD_TX_BUFFER_SIZE;
const uint8_t Family16bit::CMD_RX_DATA_ENC_KEY;
const uint8_t Family16bit::CMD_RX_DATA_BLOCK_ENCRYPTED;
const uint32_t Family16bit::CMD_ENC_KEY_ADDRESS;
const uint32_t Family16bit::CMD_ENC_DATA_ADDRESS;

Family16bit::Family16bit(bool crypto, bool f543xFam, Util *util)
{
	cryptoMode = crypto;
	Family16bit::f543xFam = f543xFam;
	Family16bit::util = util;

	address = 0;
	length = 0;
	crcInput = 0;
	baud = 0;
}

Family16bit::~Family16bit()
{

}

void Family16bit::selectRxCommand(CommandParams* cmdParams, DataBlock *dataBlock)
{
	switch (cmdParams->command)
	{
	case(Command::RX_DATA_BLOCK) :
		rxDataBlock(dataBlock);
		break;

	case(Command::JTAG_LOCK) :
		rxDataBlock(dataBlock);
		break;

	case(Command::JTAG_PASSWORD) :
		rxDataBlock(dataBlock);
		break;

	case(Command::RX_DATA_BLOCK_FAST) :
		rxDataBlockFast(dataBlock);
		break;

	case(Command::RX_PASSWORD) :
		rxPassword(&dataBlock->data);
		break;

	case(Command::RX_SECURE_DATA_BLOCK) :
		rxDataBlockSecure(dataBlock);
		break;

	default:
		throw std::runtime_error("[ERROR_MESSAGE]Command is not available for selected Family");
		break;
	}
}

void Family16bit::selectCustomCommand(CommandParams* cmdParams, std::vector<uint8_t> *buffer)
{
	switch (cmdParams->command)
	{
	case(Command::CUSTOM_COMMAND) :
		customCommand(buffer);
		break;

	default:
		throw std::runtime_error("Command is not available for selected Family");
		break;
	}
}

void Family16bit::selectCommand(CommandParams* cmdParams)
{
	address = 0;
	length = 0;
	crcInput = 0;
	baud = 0;

	switch (cmdParams->command)
	{

	case(Command::MASS_ERASE) :
		massErase();
		break;

	case(Command::CRC_CHECK) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		length = util->convertHexStringToUlong(cmdParams->params.at(1));
		if (length > 0xFFFF)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Maximum length for CRC Check is 0xFFFF!");
		}
		crcCheck(address, length);
		break;

	case(Command::SET_PC) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		setPc(address);
		break;

	case(Command::CHANGE_BAUD_RATE) :
		baud = util->convertNumStringToUlong(cmdParams->params.at(0));
		changeBaudRate(baud);
		break;

	case(Command::REBOOT_RESET) :
		rebootReset();
		break;

	case(Command::ERASE_SEGMENT) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		eraseSegment(address);
		break;

	case(Command::TOGGLE_INFO) :
		toggleInfo();
		break;

	case(Command::TX_BSL_VERSION) :
		txBslVersion();
		break;

	case(Command::TX_BUFFER_SIZE) :
		txBufferSize();
		break;

	default:
		throw std::runtime_error("Command is not available for selected Family");
		break;
	}
}
 
void Family16bit::selectTxCommand(CommandParams* cmdParams, uint32_t address, uint32_t length)
{
	switch (cmdParams->command)
	{
	case(Command::TX_DATA_BLOCK) :
		this->txDataBlock(address, length);
		break;

	default:
		break;
	}
}

std::vector<uint8_t>* Family16bit::retPtrCommandBuffer()
{
	return BaseCommands::retPtrCommandBuffer();
}

uint8_t Family16bit::retNumOfResponse(CommandParams* cmdParams)
{
	return BaseCommands::retNumOfResponse(cmdParams);
}

void Family16bit::rxDataBlockFast(DataBlock* dataBlock)
{
	clearCommandBuffer();
	commandBuffer.push_back(Family16bit::CMD_RX_DATA_BLOCK_FAST);
	commandBuffer.push_back((uint8_t)(dataBlock->startAddress & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 16) & 0xFF));
	for (uint16_t i = 0; i < dataBlock->data.size(); i++)
	{
		commandBuffer.push_back(dataBlock->data.at(i));
	}
}

void Family16bit::rxPassword(std::vector<uint8_t> *passBuffer)
{
	clearCommandBuffer();
	commandBuffer.push_back(Family16bit::CMD_RX_PASSWORD);
	for (uint16_t i = 0; i < passBuffer->size(); i++)
	{
		commandBuffer.push_back(passBuffer->at(i));
	}
}
void Family16bit::toggleInfo()
{
	clearCommandBuffer();
	commandBuffer.push_back(Family16bit::CMD_TOGGLE_INFO);
}

void Family16bit::txBslVersion()
{
	clearCommandBuffer();
	commandBuffer.push_back(Family16bit::CMD_TX_BSL_VERSION);
}

void Family16bit::txBufferSize()
{
	clearCommandBuffer();
	commandBuffer.push_back(Family16bit::CMD_TX_BUFFER_SIZE);
}

void Family16bit::rxDataBlockSecure(DataBlock* dataBlock)
{
	clearCommandBuffer();
	if (dataBlock->startAddress == Family16bit::CMD_ENC_KEY_ADDRESS)
	{
		commandBuffer.push_back(Family16bit::CMD_RX_DATA_ENC_KEY);
	}
	else if (dataBlock->startAddress == Family16bit::CMD_ENC_DATA_ADDRESS)
	{
		commandBuffer.push_back(Family16bit::CMD_RX_DATA_BLOCK_ENCRYPTED);
	}
	for (uint16_t i = 0; i < dataBlock->data.size(); i++)
	{
		commandBuffer.push_back(dataBlock->data.at(i));
	}
}