/*
==Family32bit_E4xUsbDfu.cpp==

Copyright (c) 2015–2018 Texas Instruments Incorporated

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


Family32bit_E4xxUsbDfu::Family32bit_E4xxUsbDfu(Util *util)
	: address(0),
	  blocks(0)
{
	Family32bit_E4xxUsbDfu::util = util;
}

Family32bit_E4xxUsbDfu::~Family32bit_E4xxUsbDfu()
{
	// To be implemented 
}

void Family32bit_E4xxUsbDfu::selectRxCommand(CommandParams* cmdParams, DataBlock *dataBlock)
{
	switch (cmdParams->command)
	{
	case (Command::DOWNLOAD) :
		download(dataBlock);
		break;

	case(Command::RX_DATA_BLOCK_FAST) :
		sendData(dataBlock);
		break;

	default:
		break;
	}
}

void Family32bit_E4xxUsbDfu::selectCustomCommand(CommandParams* cmdParams, std::vector<uint8_t> *buffer)
{
	throw std::runtime_error("[ERROR_MESSAGE]Command is not available for selected Family");
}

void Family32bit_E4xxUsbDfu::selectCommand(CommandParams*  cmdParams)
{
	switch (cmdParams->command)
	{
		case(Command::ERASE_BLOCK) :
			address = util->convertHexStringToUlong(cmdParams->params.at(0));
			blocks = (uint16_t)util->convertHexStringToUlong(cmdParams->params.at(1));
			eraseBlock(address, blocks);
			break;

		case(Command::MASS_ERASE) :
			massErase();
			break;

		case(Command::REBOOT_RESET) :
			rebootReset();
			break;

		default:
			throw std::runtime_error("[ERROR_MESSAGE]Command is not available for selected Family");
			break;
	}
}

void Family32bit_E4xxUsbDfu::selectTxCommand(CommandParams* cmdParams, uint32_t address, uint32_t length)
{
	switch (cmdParams->command)
	{
		case(Command::DISABLE_BIN_FILE) :
			disableBinFile();
			break;

		case(Command::TX_DATA_BLOCK_32) :
			txDataBlock32(address, length);
			break;
	
		default:
			throw std::runtime_error("[ERROR_MESSAGE]Command is not available for selected Family");
			break;
	}
}

std::vector<uint8_t>* Family32bit_E4xxUsbDfu::retPtrCommandBuffer()
{
	return &commandBuffer;
}

uint8_t Family32bit_E4xxUsbDfu::retNumOfResponse(CommandParams* cmdParams)
{
	// In this family, the response is USB standard, so return number of
	//    responses is not handled by this class 
	return 0;
}


void Family32bit_E4xxUsbDfu::download(DataBlock* dataBlock)
{
	uint16_t addressInBlock = (uint16_t)((dataBlock->startAddress >> 10) & 0x0000FFFF);
	commandBuffer.clear();
	commandBuffer.push_back(Msp432ESpecificCommand::PROG);
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
	commandBuffer.push_back((uint8_t)(addressInBlock & 0x00FF));
	commandBuffer.push_back((uint8_t)((addressInBlock >> 8)& 0x00FF));
	commandBuffer.push_back((uint8_t)(dataBlock->data.size() & 0x000000FF));
	commandBuffer.push_back((uint8_t)((dataBlock->data.size() & 0x0000FF00) >> 8));
	commandBuffer.push_back((uint8_t)((dataBlock->data.size() & 0x00FF0000) >> 16));
	commandBuffer.push_back((uint8_t)((dataBlock->data.size() & 0xFF000000) >> 24));
}

void Family32bit_E4xxUsbDfu::sendData(DataBlock* dataBlock)
{
	commandBuffer.clear();
	for (uint16_t i = 0; i < dataBlock->data.size(); i++)
	{
		commandBuffer.push_back(dataBlock->data.at(i));
	}
}

void Family32bit_E4xxUsbDfu::eraseBlock(uint32_t address, uint16_t blocks)
{
	// Maximum block number is 0x40 (64 blocks of 16kB)
	uint16_t numOfBlocks = (uint16_t)((address >> 10) & 0x000007F);
	commandBuffer.clear();
	commandBuffer.push_back(Msp432ESpecificCommand::ERASE);
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
	commandBuffer.push_back((uint8_t)(numOfBlocks & 0x00FF));
	commandBuffer.push_back((uint8_t)((numOfBlocks >> 8) & 0x00FF));
	commandBuffer.push_back((uint8_t)(blocks & 0x00FF));
	commandBuffer.push_back((uint8_t)((blocks >> 8) & 0x00FF));
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
}

void Family32bit_E4xxUsbDfu::massErase()
{
	const uint16_t startBlockAddress = 0;
	const uint16_t totalBlocks = ProgramMemorySize::MSP432E / SegmentSize::MSP432E;
	commandBuffer.clear();
	commandBuffer.push_back(Msp432ESpecificCommand::ERASE);
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
	commandBuffer.push_back((uint8_t)(startBlockAddress & 0x00FF));
	commandBuffer.push_back((uint8_t)((startBlockAddress >> 8) & 0x00FF));
	commandBuffer.push_back((uint8_t)(totalBlocks & 0x00FF));
	commandBuffer.push_back((uint8_t)((totalBlocks >> 8) & 0x00FF));
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
}

void Family32bit_E4xxUsbDfu::rebootReset()
{
	commandBuffer.clear();
	commandBuffer.resize(Msp432ECommandSize::SIZE);
	commandBuffer.at(0) = Msp432ESpecificCommand::RESET;
}

void Family32bit_E4xxUsbDfu::disableBinFile()
{
	commandBuffer.clear();
	commandBuffer.resize(Msp432ECommandSize::SIZE);
	commandBuffer.at(0) = Msp432ESpecificCommand::BIN;
	commandBuffer.at(1) = Msp432ESpecificCommand::BIN_DISABLE;
}

void   Family32bit_E4xxUsbDfu::txDataBlock32(uint32_t address, uint32_t length)
{
	uint16_t startAddressAligned = (uint16_t)((address >> 10) & 0x000007F);
	commandBuffer.clear();
	commandBuffer.push_back(Msp432ESpecificCommand::READ);
	commandBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
	commandBuffer.push_back((uint8_t)(startAddressAligned & 0x00FF));
	commandBuffer.push_back((uint8_t)((startAddressAligned & 0xFF00) >> 8));
	commandBuffer.push_back((uint8_t)(length & 0x000000FF));
	commandBuffer.push_back((uint8_t)((length & 0x0000FF00) >> 8));
	commandBuffer.push_back((uint8_t)((length & 0x00FF0000) >> 8));
	commandBuffer.push_back((uint8_t)((length & 0xFF000000) >> 8));
}