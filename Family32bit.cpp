/*
==Family32bit.cpp==

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

const uint8_t Family32bit::CMD_RX_DATA_BLOCK_32;
const uint8_t Family32bit::CMD_RX_PASSWORD_32;
const uint8_t Family32bit::CMD_ERASE_SEGMENT_32;
const uint8_t Family32bit::CMD_CRC_CHECK_32;
const uint8_t Family32bit::CMD_SET_PC_32;
const uint8_t Family32bit::CMD_TX_DATA_BLOCK_32;
const uint8_t Family32bit::CMD_TX_BSL_VERSION_32;
const uint8_t Family32bit::CMD_FACTORY_RESET;

Family32bit::Family32bit(Util *util)
{
	Family32bit::util = util;

	address = 0;
	length = 0;
	crcInput = 0;
	baud = 0;
}

Family32bit::~Family32bit()
{

}

void Family32bit::selectRxCommand(CommandParams* cmdParams, DataBlock *dataBlock)
{
	switch (cmdParams->command)
	{
	case(Command::RX_DATA_BLOCK) :
		rxDataBlock(dataBlock);
		break;

	case(Command::RX_DATA_BLOCK_32) :
		rxDataBlock32(dataBlock);
		break;

	case(Command::RX_PASSWORD_32) :
		rxPassword32(&dataBlock->data);
		break;

	default:
		throw std::runtime_error("Command is not available for selected Family");
		break;
	}
}

void Family32bit::selectCustomCommand(CommandParams* cmdParams, std::vector<uint8_t> *buffer)
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

void Family32bit::selectCommand(CommandParams*  cmdParams)
{
	address = 0;
	length = 0;
	crcInput = 0;
	baud = 0;

	switch ((*cmdParams).command)
	{
	case(Command::MASS_ERASE) :
		massErase();
		break;

	case(Command::CRC_CHECK_32) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		length = util->convertHexStringToUlong(cmdParams->params.at(1));
		if (length > 0xFFFF)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Maximum length for CRC Check is 0xFFFF!");
		}
		crcCheck32(address, length);
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

	case(Command::SET_PC_32) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		setPc32(address);
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

	case(Command::ERASE_SEGMENT_32) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		eraseSegment32(address);
		break;

	case(Command::ERASE_SEGMENT) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		eraseSegment(address);
		break;


	case(Command::TX_BSL_VERSION_32) :
		txBslVersion32();
		break;

	case(Command::FACTORY_RESET) :
		password.clear();
		if (cmdParams->params.size() == 0)
		{
			password.push_back(0xFFFFFFFF);
			password.push_back(0xFFFFFFFF);
			password.push_back(0xFFFFFFFF);
			password.push_back(0xFFFFFFFF);
		}
		else
		{
			if (cmdParams->params.size() != 4)
			{
				throw std::runtime_error("Please provide 4 password for factory reset!");
			}
			else
			{
				password.push_back(util->convertHexStringToUlong(cmdParams->params.at(0)));
				password.push_back(util->convertHexStringToUlong(cmdParams->params.at(1)));
				password.push_back(util->convertHexStringToUlong(cmdParams->params.at(2)));
				password.push_back(util->convertHexStringToUlong(cmdParams->params.at(3)));
			}
		}
		factoryReset(&password);
		break;

	default:
		throw std::runtime_error("Command is not available for seleceted Family");
		break;
	}
}
  
void Family32bit::selectTxCommand(CommandParams* cmdParams, uint32_t address, uint32_t length)
{
	switch (cmdParams->command)
	{
	case(Command::TX_DATA_BLOCK) :
		this->txDataBlock(address, length);
		break;

	case(Command::TX_DATA_BLOCK_32) :
		this->txDataBlock32(address, length);
		break;

	default:
		break;
	}
}

std::vector<uint8_t>* Family32bit::retPtrCommandBuffer()
{
	return BaseCommands::retPtrCommandBuffer();
}

uint8_t Family32bit::retNumOfResponse(CommandParams* cmdParams)
{
	return BaseCommands::retNumOfResponse(cmdParams);
}

void Family32bit::rxDataBlock32(DataBlock *dataBlock)
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_RX_DATA_BLOCK_32);
	commandBuffer.push_back((uint8_t)(dataBlock->startAddress & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 24) & 0xFF));
	for (uint16_t i = 0; i < dataBlock->data.size(); i++)
	{
		commandBuffer.push_back(dataBlock->data.at(i));
	}
}

void Family32bit::rxPassword32(std::vector<uint8_t> *passBuffer)
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_RX_PASSWORD_32);
	for (uint16_t i = 0; i < (*passBuffer).size(); i++)
	{
		commandBuffer.push_back((*passBuffer).at(i));
	}
}

void Family32bit::eraseSegment32(uint32_t address)
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_ERASE_SEGMENT_32);
	commandBuffer.push_back((uint8_t)(address & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 24) & 0xFF));
}

void Family32bit::setPc32(uint32_t address)
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_SET_PC_32);
	commandBuffer.push_back((uint8_t)(address & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 24) & 0xFF));
}

void Family32bit::txDataBlock32(uint32_t address, uint32_t length)
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_TX_DATA_BLOCK_32);
	commandBuffer.push_back((uint8_t)(address & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 24) & 0xFF));
	commandBuffer.push_back((uint8_t)(length & 0xFF));
	commandBuffer.push_back((uint8_t)((length >> 8) & 0xFF));
}

void Family32bit::crcCheck32(uint32_t address, uint32_t length)
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_CRC_CHECK_32);
	commandBuffer.push_back((uint8_t)(address & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 24) & 0xFF));
	commandBuffer.push_back((uint8_t)(length & 0xFF));
	commandBuffer.push_back((uint8_t)((length >> 8) & 0xFF));
}

void Family32bit::txBslVersion32()
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_TX_BSL_VERSION_32);
}

void Family32bit::factoryReset(std::vector<uint32_t> *pass)
{
	clearCommandBuffer();
	commandBuffer.push_back(CMD_FACTORY_RESET);
	for (uint16_t i = 0; i < 4; i++)
	{
		commandBuffer.push_back((uint8_t)(pass->at(i) & 0x000000FF));
		commandBuffer.push_back((uint8_t)((pass->at(i) >> 8) & 0x000000FF));
		commandBuffer.push_back((uint8_t)((pass->at(i) >> 16) & 0x000000FF));
		commandBuffer.push_back((uint8_t)((pass->at(i) >> 24) & 0x000000FF));
	}
}

