/*
==BaseCommands.h==

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

class BaseCommands
{	 
protected:

	/***********************************************************************************
	* CMD bytes for Base BSL Commands 
	***********************************************************************************/
	static const uint8_t CMD_RX_DATA_BLOCK = 0x10;
	static const uint8_t CMD_ERASE_SEGMENT = 0x12;
	static const uint8_t CMD_TOGGLE_INFO = 0x13;
	static const uint8_t CMD_MASS_ERASE = 0x15;
	static const uint8_t CMD_CRC_CHECK = 0x16;
	static const uint8_t CMD_SET_PC = 0x17;
	static const uint8_t CMD_TX_DATA_BLOCK = 0x18;
	static const uint8_t CMD_REBOOT_RESET = 0x25;

	/***********************************************************************************
	* Baud rate command bytes
	***********************************************************************************/
	static const uint8_t CMD_CHANGE_BAUD_RATE = 0x52;
	static const uint8_t BAUD430_9600 = 0x02;
	static const uint8_t BAUD430_19200 = 0x03;
	static const uint8_t BAUD430_38400 = 0x04;
	static const uint8_t BAUD430_57600 = 0x05;
	static const uint8_t BAUD430_115200 = 0x06;

	/***********************************************************************************
	*Class variables
	***********************************************************************************/
	Util *util;
	uint32_t address;
	uint32_t length;
	uint32_t crcInput;
	uint32_t baud;
 	std::vector <uint32_t> password;
	std::vector<uint8_t> commandBuffer;

	/***********************************************************************************
	*Function:    clearCommandBuffer
	*Description: clear the commandBuffer before it is initialized with new command
	              for the new transmission
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void clearCommandBuffer();

	/***********************************************************************************
	*Function:    retPtrCommandBuffer
	*Description: return the pointer to commandBuffer to be handled in transmission
	              class
	*Parameters:  -
	*Returns:	  std::vector<uint8_t>*
	***********************************************************************************/
	std::vector<uint8_t>* retPtrCommandBuffer();

	/***********************************************************************************
	*Function:    retNumOfResponse
	*Description: return number of response of specific command. this is applicable and
	              used for the USB protocol, since the communication has to know
				  how many bytes exactly the bytes will be transmitted from BSL to the
				  PC
	class
	*Parameters:  -
	*Returns:	  uint8_t
	***********************************************************************************/
	uint8_t retNumOfResponse(CommandParams* cmdParams);
  	
	/***********************************************************************************
	*General command for all Families
	***********************************************************************************/
	/***********************************************************************************
	*Function:    rxDataBlock
	*Description: create the RX_DATA_BLOCK command in commandBuffer
	*Parameters:  DataBlock* dataBlock from ScripterStream class, as the result of
	              reading an input file
	*Returns:	  -
	***********************************************************************************/
	void rxDataBlock(DataBlock* dataBlock);

	/***********************************************************************************
	*Function:    massErase
	*Description: create the MASS_ERASE command in commandBuffer
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void massErase();

	/***********************************************************************************
	*Function:    rebootReset
	*Description: create the REBOOT_RESET command in commandBuffer
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void rebootReset();

	/***********************************************************************************
	*Function:    crcCheck
	*Description: create the CRC_CHECK command in commandBuffer
	*Parameters:  uint32_t address: start address where the crc calculation starts
	              uint32_t length: number of bytes that will be crc-calculated
	*Returns:	  -
	***********************************************************************************/
	void crcCheck(uint32_t address, uint32_t length);

	/***********************************************************************************
	*Function:    eraseSegment
	*Description: create the ERASE_SEGMENT command in commandBuffer
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void eraseSegment(uint32_t address);

	/***********************************************************************************
	*Function:    setPc
	*Description: create the SET_PC command	in commandBuffer
	*Parameters:  uint32_t pc: address to be set in PC register
	*Returns:	  -
	***********************************************************************************/
	void setPc(uint32_t pc);

	/***********************************************************************************
	*Function:    txDataBlock
	*Description: create the TX_DATA_BLOCK command in commandBuffer
	*Parameters:  uint32_t address: start address of the memory to be read
	              uint32_t length: number of bytes to be read from start address
	*Returns:	  -
	***********************************************************************************/
	void txDataBlock(uint32_t address, uint32_t length);

	/***********************************************************************************
	*Function:    changeBaudRate
	*Description: create the CHANGE_BAUD command in commandBuffer
	*Parameters:  uint32_t baudRate: new baudrate to be set
	*Returns:	  -
	***********************************************************************************/
	void changeBaudRate(uint32_t baudRate);

	/***********************************************************************************
	*Function:    changeBaudRate
	*Description: create the CUSTOM_COMMAND command in commandBuffer
	*Parameters:  std::vector<uint8_t> *buffer: pointer to buffer that contains data
	              to be sent, the buffer is the result from reading input file 
				  in ScripterStream
	*Returns:	  -
	***********************************************************************************/
	void customCommand(std::vector<uint8_t> *buffer);
};