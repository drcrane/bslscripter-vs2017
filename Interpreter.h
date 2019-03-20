/*
==Interpreter.h==

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

class Interpreter
{
public:

	/***********************************************************************************
	* Constructor
	***********************************************************************************/
	Interpreter(int argc, char * argv[], Util *util, ScripterStream *scripterStream);
	
	/***********************************************************************************
	* Destructor
	***********************************************************************************/
	~Interpreter();

	/***********************************************************************************
	*Function:	   mainControllerScript
	*Description:  main controller that handles script mode
	*Parameters:   -
	*Returns:	   -
	***********************************************************************************/
	void mainControllerScript();

	/***********************************************************************************
	*Function:	   mainControllerScript
	*Description:  main controller that handles command line interface
	*Parameters:   -
	*Returns:	   int8_t
	***********************************************************************************/
	int8_t mainControllerCli(int argc, char * argv[]);

	/***********************************************************************************
	*Function:	   matchCommand
	*Description:  receive the command and match it with the defined commands
	*Parameters:   std::string line: input line
	*Returns:	   -
	***********************************************************************************/
	void matchCommand(std::string line);

	/***********************************************************************************
	*Function:	   executeCommand
	*Description:  execute command based on the given command and parameters
	*Parameters:   CommandParams cmd: command and its parameters
	*Returns:	   -
	***********************************************************************************/
	void executeCommand(CommandParams cmd);

	/***********************************************************************************
	*Function:	   showVerbose
	*Description:  -
	*Returns:	   -
	***********************************************************************************/
	void showVerbose(bool enVerbose);

private:

	/***********************************************************************************
	* Class variables
	***********************************************************************************/
	Util *util;
	ScripterStream *scripterStream;

	Timer timer;
	double elapsedTime;

	std::string mainPathDir;
	bool enLog;
	bool enVerbose;
	bool enQuiet;
	
	uint16_t lineNum;
	std::vector<std::string> lineTokens;

	uint8_t numToEnum;
	CommandParams commandParams;
	ModeParams modeParams;

	std::string exitSpecsInfo();
	std::string argCmd;
	std::string cliParam;
	std::vector<std::string> cliParamsVector;

	bool jtagLockWithPwd;
	uint32_t numOfResponse;

	FactoryCommunication factoryCommunication;
	FactoryTarget factoryTarget;

	std::unique_ptr<TargetInterface> ptrTarget;
	std::unique_ptr<CommInterface> ptrComm;

	static const std::string MAC_ARG;
	static const std::string IP_CLIENT_ARG;
	static const std::string IP_SERVER_ARG;
	static const std::string I2C_ADDRESS;
	static const std::string PARITY_ARG;
	static const std::string CRYPTO_ARG;
	static const std::string MSPFET1_ARG;
	static const std::string MSPFET2_ARG;
	static const std::string DISABLE_AUTO_BAUD;

	/***********************************************************************************
	*Function:	  interpretMode
	*Description: interprets the MODE command parameters and initialize the device
	                and communication protocol
	*Parameters:  std::vector<std::string> *params: given parameters
	*Returns:	  -
	***********************************************************************************/
	void interpretMode(std::vector<std::string> *params);

	/***********************************************************************************
	*Function:	  callFactory
	*Description: call the factory to create the communication and target interface
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void callFactory();

	/***********************************************************************************
	*Function:	  executeRxCommand
	*Description: select the rx command based on the given command
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeRxCommand();

	/***********************************************************************************
	*Function:	  executeTxCommand
	*Description: select the tx command based on the given command
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeTxCommand();

	/***********************************************************************************
	*Function:	  executeTxCommandE4xx
	*Description: select the tx command based on the given command specific for MSP432E
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeTxCommandE4xx();

	/***********************************************************************************
	*Function:	  executePcCommand
	*Description: select the pc command based on the given command
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executePcCommand();

	/***********************************************************************************
	*Function:	  executeOtherCommand
	*Description: select the other command based on the given command
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeOtherCommand();

	/***********************************************************************************
	*Function:	  executeOtherCommandE4xx
	*Description: select the other command based on the given command specific
	              for MSP432E4xx
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeOtherCommandE4xx();

	/***********************************************************************************
	*Function:	  getNumOfResponse
	*Description: call and process the retNumOfResponse for TargetInterface
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void getNumOfResponse();

	/***********************************************************************************
	*Function:	  getAndTranslateResponse
	*Description: call and process the processResponse for TargetInterface
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void getAndTranslateResponse();

	/***********************************************************************************
	*Function:	  showResponse
	*Description: show the response message 
	*Parameters:  bool cliQuiet: if quiet, nothing will be shown
	*Returns:	  -
	***********************************************************************************/
	void showResponse(bool cliQuiet);

	/***********************************************************************************
	*Function:	  executeJtagLock
	*Description: execute the JTAG lock command, as part of executeRxCommand
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeJtagLock();

	/***********************************************************************************
	*Function:	  executeJtagPasswordSetup
	*Description: execute the JTAG password setup command, as part of executeRxCommand
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeJtagPasswordSetup();

	/***********************************************************************************
	*Function:	  executeDefaultPasswordCommand
	*Description: execute the default RX_PASSWORD, as part of executeRxCommand
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeDefaultPasswordCommand();

	/***********************************************************************************
	*Function:	  executeCustomCommand
	*Description: execute CUSTOM_COMMAND, as part of executeRxCommand
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeCustomCommand();

	/***********************************************************************************
	*Function:	  executeVersion
	*Description: execute APP_VERSION in script mode
	*             execute --version or -a in CLI mode
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void executeAppVersion();
};


