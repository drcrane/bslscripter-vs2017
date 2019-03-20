/*
==CommInterface.h==

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

class CommInterface 
{
public:

	/***********************************************************************************
	* Destructor
	***********************************************************************************/
	virtual ~CommInterface()
	{

	}

 	/***********************************************************************************
	*Function:    init
	*Description: do the initialization of the communication protocol based
	*             on the parameters of the given command
	*Parameters:  ActualCommandParams actualCommandParams: parameters from interpreter
	*Returns:     -
	***********************************************************************************/
	virtual void init(ModeParams* modeParams) = 0;
	
	/***********************************************************************************
	*Function:    transmitBuffer
	*Description: send the BSL command 
	*Parameters:  std::vector<uint8_t>* txBuffer: prepared buffer from Target class
	*Returns:     -
	***********************************************************************************/
	virtual void transmitBuffer(std::vector<uint8_t>* txBuffer) = 0;

	/***********************************************************************************
	*Function:	  receiveBuffer
	*Description: receive byte(s) response from target device
	*Parameters:  int32_t size : number of bytes that should be received by the
	*                            scripter
	*             (-1) means only one byte response without wrapped in BSL data packet
	*             (0) means no byte response
	*			  (n>0) means n bytes wrapped in BSL data packet 
	*Returns:     std::vector<uint8_t>*
	***********************************************************************************/
	virtual std::vector<uint8_t>* receiveBuffer(int32_t size) = 0;

	/***********************************************************************************
	*Function:    changeBaudRate
	*Description: change baud rate with the given baudRate on the serial communication
	*Parameters:  uint32_t baudRate: new baudRate 
	*Returns:	  -
	***********************************************************************************/
	virtual void changeBaudRate(uint32_t baudRate) = 0;

	/***********************************************************************************
	*Function:	  close
	*Description: close communication port
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	virtual void close() = 0;

	/***********************************************************************************
	*Function:	  resetBytesWritten
	*Description: reset bytes written in comm port
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	virtual void resetBytesWritten() = 0;

	/***********************************************************************************
	*Function:	  retBytesWritten
	*Description: return number of bytes sent including the additional bytes
	*Parameters:  -
	*Returns:     uint32_t
	***********************************************************************************/
	virtual uint32_t retBytesWritten() = 0;

	/***********************************************************************************
	*Function:	  resetBytesWritten
	*Description: reset bytes written in comm port
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	virtual void resetBytesSent() = 0;

	/***********************************************************************************
	*Function:	  retBytesWritten
	*Description: return number of bytes sent including the additional bytes
	*Parameters:  -
	*Returns:     uint32_t
	***********************************************************************************/
	virtual uint32_t retBytesSent() = 0;

	/***********************************************************************************
	*Function:	  retBytesRead
	*Description: return number of bytes received without the additional bytes
	*Parameters:  -
	*Returns:     uint32_t
	***********************************************************************************/
	virtual uint32_t retBytesRead() = 0;

	/***********************************************************************************
	*Function:	  retAck 
	*Description: return received ACK byte
	*Parameters:  -
	*Returns:     uint8_t
	***********************************************************************************/
	virtual uint8_t retAck() = 0;

	/***********************************************************************************
	*Function:	  retLastRxBufferIdx
	*Description: return last index on the rxBuffer
	*Parameters:  -
	*Returns:	  uint32_t
	***********************************************************************************/
	virtual uint32_t retLastRxBufferIdx() = 0;

	/***********************************************************************************
	*Function:	  retPtrRxBuffer
	*Description: return the pointer of rxBuffer
	*Parameters:  -
	*Returns:	  std::vector<uint8_t>*
	***********************************************************************************/
	virtual std::vector<uint8_t>* retPtrRxBuffer() = 0;

	/***********************************************************************************
	*Function:	  retPtrTxBuffer
	*Description: return the pointer of txBuffer
	*Parameters:  -
	*Returns:	  std::vector<uint8_t>*
	***********************************************************************************/
	virtual std::vector<uint8_t>* retPtrTxBuffer() = 0;

	/***********************************************************************************
	*Function:	  resetIdxAcc
	*Description: reset index calculation for the rxBuffer for reading memory process
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	virtual void resetIdxAcc() = 0;

	/***********************************************************************************
	*Function:	  processResponse
	*Description: calling processResponse from the inhereted class BslResponseHandler
	*Parameters:  uint8_t ack: received ACK byte
	              std::vector<uint8_t> *response: pointer to rxBuffer
	*Returns:	  -
	***********************************************************************************/
	virtual void processResponse(uint8_t ack, std::vector<uint8_t> *response) = 0;

	/***********************************************************************************
	*Function:	  retErrorAck
	*Description: calling retErrorAck from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	virtual bool retErrorAck() = 0;

	/***********************************************************************************
	*Function:	  retErrorStatus
	*Description: calling retErrorStatus from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	virtual bool retErrorStatus() = 0;

	/***********************************************************************************
	*Function:	  retErrorAckMessage
	*Description: calling retErrorAckMessage from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  std::string 
	***********************************************************************************/
	virtual std::string retErrorAckMessage() = 0;

	/***********************************************************************************
	*Function:	  retErrorStatusMessage
	*Description: calling retErrorStatusMessage from the inhereted class
	                BslResponseHandler
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	virtual std::string retErrorStatusMessage() = 0;

	/***********************************************************************************
	*Function:	  retCrcCheckResult
	*Description: calling retCrcCheckResult from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	virtual bool retCrcCheckResult() = 0;

	/***********************************************************************************
	*Function:	  translateResponse
	*Description: calling translateResponse from the inhereted class BslResponseHandler
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	virtual std::string translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx) = 0;
};