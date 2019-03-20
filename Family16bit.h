/*
==Family16bit.h==

Copyright (c) 2015�2017 Texas Instruments Incorporated

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
(�TI Devices�). No hardware patent is licensed hereunder.

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

THIS SOFTWARE IS PROVIDED BY TI AND TI�S LICENSORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI�S LICENSORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#pragma once

class Family16bit : public  TargetInterface, public BaseCommands
{
public:

	/*******************************************************************************
	* Constructor
	********************************************************************************/
	Family16bit(bool crypto, bool f543x, Util *util);
	
	/*******************************************************************************
	* Destructor
	********************************************************************************/
	~Family16bit();

	/*******************************************************************************
	*Function:    selectRxCommand
	*Description: prepare the commandBuffer for Rx Command 
	              the Rx command is related with the command which has the
				  data block input from ScripterStream class
	*Parameters:  CommandParams* cmdParams: interpreted command Params 
	              DataBlock *dataBlock : data bytes from reading in ScripterStream
	*Returns:     -
	*******************************************************************************/
	void selectRxCommand(CommandParams* cmdParams, DataBlock *dataBlock) OVERRIDE;

	/*******************************************************************************
	*Function:    selectCustomCommand
	*Description: prepare the commandBuffer with CUSTOM_COMMAND 
	*Parameters:  CommandParams* cmdParams: interpreted command Params
	              std::vector<uint8_t>* buffer : data buffer from reading 
				    in ScripterStream
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
	              the Tx command is related with the command which will write the
				  received bytes into a file
	*Parameters:  CommandParams* cmdParams: interpreted command Params
	              uint32_t address: start address in the memory which will be read
	              uint32_t length: number of bytes will be read
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
	* Command bytes for  Family16bit BSL Commands
	********************************************************************************/
	static const uint8_t CMD_RX_DATA_BLOCK_FAST = 0x1B;
	static const uint8_t CMD_RX_PASSWORD = 0x11;
	static const uint8_t CMD_TOGGLE_INFO = 0x13;
	static const uint8_t CMD_TX_BSL_VERSION = 0x19;
	static const uint8_t CMD_TX_BUFFER_SIZE = 0x1A;
	static const uint8_t CMD_RX_DATA_ENC_KEY = 0x31;
	static const uint8_t CMD_RX_DATA_BLOCK_ENCRYPTED = 0x30;
	static const uint32_t CMD_ENC_KEY_ADDRESS = 0x5000;
	static const uint32_t CMD_ENC_DATA_ADDRESS = 0xA000;
	
	/*******************************************************************************
	* Class variables
	********************************************************************************/
	bool cryptoMode;
	bool f543xFam;

	/*******************************************************************************
	* Specific Family16bit Commands
	********************************************************************************/
	/*******************************************************************************
	*Function:    rxDataBlockFast
	*Description: prepare commandBuffer for RX_DATA_BLOCK_FAST command
	*Parameters:  DataBlock* dataBlock: data bytes from reading input file in 
	                 ScripterStream
	*Returns:     -
	*******************************************************************************/
	void rxDataBlockFast(DataBlock* dataBlock);

	/*******************************************************************************
	*Function:    rxPassword
	*Description: prepare commandBuffer for RX_PASSWORD command
	*Parameters:  std::vector<uint8_t> *passBuffer: data bytes from reading input file 
	                in ScripterStream or by default password
	*Returns:     -
	*******************************************************************************/
	void rxPassword(std::vector<uint8_t> *passBuffer);

	/*******************************************************************************
	*Function:    toggleInfo
	*Description: prepare commandBuffer for TOGGLE_INFO command
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void toggleInfo();

	/*******************************************************************************
	*Function:    txBslVersion
	*Description: prepare commandBuffer for TX_BSL_VERSION command
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void txBslVersion();

	/*******************************************************************************
	*Function:    txBufferSize
	*Description: prepare commandBuffer for TX_BUFFER_SIZE command
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void txBufferSize();

	/*******************************************************************************
	*Function:    rxDataBlockSecure
	*Description: prepare commandBuffer for RX_SECURE_DATA_BLOCK command
	*Parameters:  DataBlock* dataBlock: data bytes from reading input file in 
	                 ScripterStream
	*Returns:     -
	*******************************************************************************/
	void rxDataBlockSecure(DataBlock* dataBlock);
};