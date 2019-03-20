/*
==Family32bit_E4xx.cpp==

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

const uint8_t Family32bit_E4xx::CMD_DOWNLOAD;
const uint8_t Family32bit_E4xx::CMD_RUN;
const uint8_t Family32bit_E4xx::CMD_SEND_DATA;
const uint8_t Family32bit_E4xx::CMD_RESET;

Family32bit_E4xx::Family32bit_E4xx(Util *util)
{
	Family32bit_E4xx::util = util;
	address = 0;
	length = 0;
}

Family32bit_E4xx::~Family32bit_E4xx()
{

}

void Family32bit_E4xx::selectRxCommand(CommandParams* cmdParams, DataBlock *dataBlock)
{
	switch (cmdParams->command)
	{
	case (Command::DOWNLOAD) :
		//For MSP432E family, the first command to download the firmware image is the
		//   DOWNLOAD command that contains of parameters of address that would like
		//   to be programmed and also the size. The address and size has to be aligned with
		//   4kB segment size of the MSP432E memory segment alignment.
		download(dataBlock);
		totalDataToBeProgrammed = dataBlock->data.size();
		totalDataToBeSentInOnePacket = 0;
		totalDataHasBeenSent = 0;		  

		ptrDataBlockRead = dataBlock->data.data();
		break;

	case(Command::RX_DATA_BLOCK_32) :
		if (totalDataToBeProgrammed > BlockDataSize::MSP432E_SERIAL)
		{
			//packet with full BlockDataSize::MSP432E_SERIAL bytes
			totalDataToBeSentInOnePacket = BlockDataSize::MSP432E_SERIAL;
		}
		else
		{
			//last packet
			totalDataToBeSentInOnePacket = totalDataToBeProgrammed;
		}
		sendData(ptrDataBlockRead, totalDataToBeSentInOnePacket);

		ptrDataBlockRead += totalDataToBeSentInOnePacket;
		//Send data has no input from outside variable
		//It has its own FSM to calculate the pointer to the 
		//renew variables value for the next sending
		totalDataToBeProgrammed -= totalDataToBeSentInOnePacket;
		totalDataHasBeenSent += totalDataToBeSentInOnePacket;
		break;

	default:
		throw std::runtime_error("[ERROR_MESSAGE]Command is not available for seleceted Family");
		break;

	}
}

void Family32bit_E4xx::selectCustomCommand(CommandParams* cmdParams, std::vector<uint8_t> *buffer)
{
	throw std::runtime_error("Command is not available for selected Family");
}

void Family32bit_E4xx::selectCommand(CommandParams*  cmdParams)
{
	switch ((*cmdParams).command)
	{
	case(Command::REBOOT_RESET) :
		reset();
		break;

	case(Command::SET_PC_32) :
		address = util->convertHexStringToUlong(cmdParams->params.at(0));
		run(address);
		break;

	default:
		throw std::runtime_error("[ERROR_MESSAGE]Command is not available for seleceted Family");
		break;
	}	
}

void Family32bit_E4xx::selectTxCommand(CommandParams* cmdParams, uint32_t address, uint32_t length)
{
	throw std::runtime_error("[ERROR_MESSAGE]Command is not available for selected Family");
}

std::vector<uint8_t>* Family32bit_E4xx::retPtrCommandBuffer()
{
	return &commandBuffer;
}

uint8_t Family32bit_E4xx::retNumOfResponse(CommandParams* cmdParams)
{
	switch (cmdParams->command)
	{
	case(Command::REBOOT_RESET) :
	case(Command::SET_PC_32) :
	case(Command::DOWNLOAD) : 
	case(Command::RX_DATA_BLOCK_32) :
		return 2;
		break;
	default:
		return 0;
		break;
	}
}

void Family32bit_E4xx::download(DataBlock *dataBlock)
{
	commandBuffer.clear();
	commandBuffer.push_back(CMD_DOWNLOAD);
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 24) & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->startAddress >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)(dataBlock->startAddress & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->data.size() >> 24) & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->data.size() >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((dataBlock->data.size() >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)(dataBlock->data.size() & 0xFF));
}

void Family32bit_E4xx::run(uint32_t address)
{
	commandBuffer.clear();
	commandBuffer.push_back(CMD_RUN);
	commandBuffer.push_back((uint8_t)((address >> 24) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 16) & 0xFF));
	commandBuffer.push_back((uint8_t)((address >> 8) & 0xFF));
	commandBuffer.push_back((uint8_t)(address & 0xFF));
}
 
void Family32bit_E4xx::sendData(uint8_t *ptrData, uint32_t size)
{
	commandBuffer.clear();
	commandBuffer.push_back(CMD_SEND_DATA);
	for (uint8_t i = 0; i < size; i++)
	{
		commandBuffer.push_back(*ptrData++);
	}
}

void Family32bit_E4xx::reset()
{
	commandBuffer.clear();
	commandBuffer.push_back(CMD_RESET);
}
