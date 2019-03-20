/*
==BaseCommands.cpp==

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

const uint8_t BaseCommands::CMD_RX_DATA_BLOCK;
const uint8_t BaseCommands::CMD_ERASE_SEGMENT;
const uint8_t BaseCommands::CMD_TOGGLE_INFO;
const uint8_t BaseCommands::CMD_MASS_ERASE;
const uint8_t BaseCommands::CMD_CRC_CHECK;
const uint8_t BaseCommands::CMD_SET_PC;
const uint8_t BaseCommands::CMD_TX_DATA_BLOCK;
const uint8_t BaseCommands::CMD_REBOOT_RESET;

const uint8_t BaseCommands::CMD_CHANGE_BAUD_RATE;
const uint8_t BaseCommands::BAUD430_9600;
const uint8_t BaseCommands::BAUD430_19200;
const uint8_t BaseCommands::BAUD430_38400;
const uint8_t BaseCommands::BAUD430_57600;
const uint8_t BaseCommands::BAUD430_115200;

void BaseCommands::clearCommandBuffer()
{
	commandBuffer.clear();
}

std::vector<uint8_t>* BaseCommands::retPtrCommandBuffer()
{
	return &commandBuffer;
}

uint8_t BaseCommands::retNumOfResponse(CommandParams* cmdParams)
{
	uint8_t numOfResponse = 0;

	switch (cmdParams->command)
	{
		case(Command::CRC_CHECK) :
			numOfResponse = 2;
			break;

		case(Command::TX_BSL_VERSION) :
			numOfResponse = 4;
			break;

		case (Command::SET_PC) :
			numOfResponse = 0;
			break;

		case (Command::REBOOT_RESET) :
			numOfResponse = 0;
			break;

		case (Command::RX_DATA_BLOCK_FAST) :
			numOfResponse = 0;
			break;

		default:
			numOfResponse = 1;
			break;
	}
	return numOfResponse;
}

void BaseCommands::rxDataBlock(DataBlock* dataBlock)
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_RX_DATA_BLOCK);
	commandBuffer.push_back((uint8_t)(dataBlock->startAddress & 0xFF));
	commandBuffer.push_back((uint8_t)(((dataBlock->startAddress >> 8) & 0xFF)));
	commandBuffer.push_back((uint8_t)(((dataBlock->startAddress >> 16) & 0xFF)));
	for (uint16_t i = 0; i < dataBlock->data.size(); i++)
	{
		commandBuffer.push_back(dataBlock->data.at(i));
	}
}

void BaseCommands::massErase()
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_MASS_ERASE);
}

void BaseCommands::rebootReset()
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_REBOOT_RESET);
}


void BaseCommands::crcCheck(uint32_t address, uint32_t length)
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_CRC_CHECK);
	commandBuffer.push_back((uint8_t)(address & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)(length & 0xFF));
	commandBuffer.push_back((uint8_t)((length >> 8) & 0xFF));
}

void BaseCommands::eraseSegment(uint32_t address)
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_ERASE_SEGMENT);
	commandBuffer.push_back((uint8_t)(address & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
}


void BaseCommands::setPc(uint32_t pc)
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_SET_PC);
	commandBuffer.push_back((uint8_t)(pc & 0xFF));
	commandBuffer.push_back((uint8_t)((pc >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((pc >> 16) & 0xFF));
}

void BaseCommands::txDataBlock(uint32_t address, uint32_t length)
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_TX_DATA_BLOCK);
	commandBuffer.push_back((uint8_t)(address & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)(length & 0xFF));
	commandBuffer.push_back((uint8_t)((length >> 8) & 0xFF));
}

void BaseCommands::changeBaudRate(uint32_t baudRate)
{
	clearCommandBuffer();
	commandBuffer.push_back(BaseCommands::CMD_CHANGE_BAUD_RATE);
	switch (baudRate)
	{
	case(9600) :
		commandBuffer.push_back(BaseCommands::BAUD430_9600);
		break;
	case(19200) :
		commandBuffer.push_back(BaseCommands::BAUD430_19200);
		break;
	case(38400) :
		commandBuffer.push_back(BaseCommands::BAUD430_38400);
		break;
	case(57600) :
		commandBuffer.push_back(BaseCommands::BAUD430_57600);
		break;
	case(115200) :
		commandBuffer.push_back(BaseCommands::BAUD430_115200);
		break;
	default:
		throw std::runtime_error("[ERROR_MESSAGE]Baud rate input is invalid!");
		break;
	}
}

void BaseCommands::customCommand(std::vector<uint8_t> *buffer)
{
	clearCommandBuffer();
	commandBuffer.push_back(buffer->at(0));
	for (uint16_t i = 1; i <buffer->size(); i++)
	{
		commandBuffer.push_back(buffer->at(i));
	}
}



