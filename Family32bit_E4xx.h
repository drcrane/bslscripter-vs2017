/*
==Family32bit_E4xx.h==

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

class Family32bit_E4xx : public TargetInterface
{
public:

	/***********************************************************************************
	* Constructor
	***********************************************************************************/
	Family32bit_E4xx(Util *util);

	/***********************************************************************************
	* Destructor
	***********************************************************************************/
	~Family32bit_E4xx();

	/*******************************************************************************
	*Function:    selectRxCommand
	*Description: prepare the commandBuffer for Rx Command
	*             the Rx command is related with the command which has the
	*             data block input from ScripterStream class
	*Parameters:  CommandParams* cmdParams: interpreted command Params
	*             DataBlock *dataBlock : data bytes from reading in ScripterStream
	*Returns:     -
	*******************************************************************************/
	void selectRxCommand(CommandParams* cmdParams, DataBlock *dataBlock) OVERRIDE;

	/*******************************************************************************
	*Function:    selectCustomCommand
	*Description: prepare the commandBuffer with CUSTOM_COMMAND
	*Parameters:  CommandParams* cmdParams: interpreted command Params
	*             std::vector<uint8_t>* buffer : data buffer from reading
	*             in ScripterStream
	*Returns:     -
	*******************************************************************************/
	void selectCustomCommand(CommandParams* cmdParams, std::vector<uint8_t>* buffer) OVERRIDE;

	/*******************************************************************************
	*Function:    selectCommand
	*Description: prepare the commandBuffer depend on the given cmdParams
	*Parameters:  CommandParams* cmdParams: interpreted command Params
	*Returns:     -
	*******************************************************************************/
	void selectCommand(CommandParams* cmdParams) OVERRIDE;

	/*******************************************************************************
	*Function:    selectTxCommand
	*Description: prepare the commandBuffer for Tx Command
	*             the Tx command is related with the command which will write the
	*             received bytes into a file
	*Parameters:  CommandParams* cmdParams: interpreted command Params
	*             uint32_t address: start address in the memory which will be read
	*             uint32_t length: number of bytes will be read
	*Returns:     -
	*******************************************************************************/
	void selectTxCommand(CommandParams* cmdParams, uint32_t address, uint32_t length) OVERRIDE;

	/*******************************************************************************
	*Function:    retPtrCommandBuffer
	*Description: calling the retPtrCommandBuffer from BaseClass
	*Parameters:  -
	*Returns:     std::vector<uint8_t>*
	*******************************************************************************/
	std::vector<uint8_t>* retPtrCommandBuffer() OVERRIDE;

	/*******************************************************************************
	*Function:    retNumOfResponse
	*Description: calling the retNumOfResponse from BaseClass
	*Parameters:  -
	*Returns:     std::vector<uint8_t>*
	*******************************************************************************/
	uint8_t retNumOfResponse(CommandParams* cmdParams) OVERRIDE;

private:

	/*******************************************************************************
	* Command bytes for  Family32bit_E4xx BSL Commands
	********************************************************************************/
	static const uint8_t CMD_DOWNLOAD = 0x21;
	static const uint8_t CMD_RUN = 0x22;
	static const uint8_t CMD_SEND_DATA = 0x24;
	static const uint8_t CMD_RESET = 0x25;

	/***********************************************************************************
	*Class variables
	***********************************************************************************/
	Util *util;
	uint32_t address;
	uint32_t length;
	std::vector<uint8_t> commandBuffer;

	uint8_t* ptrDataBlockRead;

	uint32_t totalDataToBeProgrammed;
	uint32_t totalDataToBeSentInOnePacket;
	uint32_t totalDataHasBeenSent;

	/*******************************************************************************
	* Specific Family32bit_E4xx Commands
	********************************************************************************/
	/*******************************************************************************
	*Function:	   download
	*Description:  Download command sends the address that will be programmed and 
	*              the size of the block. Address and size has to be aligned with
	*              target sector size (16kB), and this is already handled by the 
	*              scripter stream
	*Parameters:   DataBlock *dataBlock: pointer to the dataBlock
	*Returns:     -
	*******************************************************************************/
	void download(DataBlock *dataBlock);

	/***********************************************************************************
	*Function:	   sendData
	*Description:  after the download command sent, the sendData will prepare the data
	*              into command buffer. Maximum bytes in one packet is 60 bytes.
	*Parameters:   uint8_t *ptrData: pointer to the buffer
	*              uint32_t size: number of bytes to be copied to the commandBuffer
	*Returns:	  -
	***********************************************************************************/
	void sendData(uint8_t *ptrData, uint32_t size);

	/***********************************************************************************
	*Function:	  run
	*Description: create the SET_PC_32 command	in commandBuffer
	*Parameters:  uint32_t pc: address to be set in PC register
	*Returns:	  -
	***********************************************************************************/
	void run(uint32_t address);

	/***********************************************************************************
	*Function:	  reset
	*Description: create the REBOOT_RESET command in commandBuffer
	*Parameters:
	*Returns:	  -
	***********************************************************************************/
	void reset();
};