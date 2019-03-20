/*
==Interpreter.cpp==

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


const std::string Interpreter::MAC_ARG = "MAC=";
const std::string Interpreter::IP_CLIENT_ARG = "IPClient=";
const std::string Interpreter::IP_SERVER_ARG = "IPServer=";
const std::string Interpreter::I2C_ADDRESS = "I2CAddr=";
const std::string Interpreter::PARITY_ARG = "PARITY";
const std::string Interpreter::INVOKE_ARG = "INVOKE";
const std::string Interpreter::CRYPTO_ARG = "CRYPTO";
const std::string Interpreter::MSPFET1_ARG = "MSPFET";
const std::string Interpreter::MSPFET2_ARG = "MSP-FET";
const std::string Interpreter::DISABLE_AUTO_BAUD = "DISABLE_AUTO_BAUD";

Interpreter::Interpreter(int argc, char * argv[], Util *util, ScripterStream *scripterStream)
{
	//initiate the pointer to the built Util class from main
	Interpreter::util = util;
	Interpreter::scripterStream = scripterStream;
    jtagLockWithPwd = false;
    
	if (scripterStream->retCliMode())
	{
		mainPathDir = scripterStream->startScripter(argv[1]);
		mainControllerCli(argc, argv);
 		enQuiet = false;
	}
	else
	{
		mainPathDir = scripterStream->startScripter(argv[1]);
		enLog = false;
		enVerbose = false;
		enQuiet = false;
		lineNum = 0;
		mainControllerScript();
	}
}

Interpreter::~Interpreter()
{

}

int8_t Interpreter::mainControllerCli(int argc, char * argv[])
{
	boost::program_options::options_description desc("Options");
	desc.add_options()
		("help,h", "Display usage information")
		("version,a" , "BSL Scripter version")
		("?", "Display usage information")
		("exitInfo,x", "Exit specification")
		("device,n", boost::program_options::value<std::string>(&cliParam)->implicit_value("Flash"), "Device family setting")
		("log,g", "Logging mode")
		("quiet,q", "Quiet mode")
		("debug,d", "Debug/Verbose mode")
		("initComm,i", boost::program_options::value<std::string>(&cliParam), "Communication port and protocol initialization")
		("bslPwd,b", boost::program_options::value<std::string>(&cliParam)->implicit_value(""), "Unlock BSL")
		("speed,j", boost::program_options::value<std::string>(&cliParam)->implicit_value("Slow"), "Speed setting")
		("unlockInfo,u", "Unlock information memory")
		("erase,e", boost::program_options::value<std::string>(&cliParam)->implicit_value("ERASE_ALL"), "Erase memory")
		("write,w", boost::program_options::value<std::string>(&cliParam), "Write memory")
		("read,r", boost::program_options::value<std::string>(&cliParam), "Read memory")
		("verify,v", boost::program_options::value<std::string>(&cliParam)->implicit_value(""), "Verify memory")
		("lockJtag,l",  "Lock JTAG")
		("pwdLockJtag,p", boost::program_options::value<std::string>(&cliParam)->implicit_value(""), "Lock JTAG Password")
		("exit,z", boost::program_options::value<std::string>(&cliParam)->implicit_value(""), "Exit scripter");

	boost::program_options::variables_map vm;

	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		/***********************************************************************************
		* Help options
		***********************************************************************************/
		// Execute --help / -h / --?
		if (vm.count("help") || (vm.count("?")))
		{
			std::cout << "BSL Scripter Command Line Parameters" << std::endl << desc << std::endl;
			return 0;
		}

		// Execute --exitInfo / -x 
		if (vm.count("exitInfo"))
		{
			std::cout << exitSpecsInfo() << std::endl;
			return 0;
		}

		/***********************************************************************************
		* Version options
		***********************************************************************************/
		// Execute --version / -a
		if (vm.count("version"))
		{
			executeAppVersion();
		}

		/***********************************************************************************
		* Quiet option
		***********************************************************************************/
		if (vm.count("quiet"))
		{
			scripterStream->setEnQuiet(true);
		}
		else
		{
			scripterStream->setEnQuiet(false);
		}

		scripterStream->showHeaderFileStatus();

		/***********************************************************************************
		* Logging option
		***********************************************************************************/
		if (vm.count("log"))
		{
			scripterStream->writeLogFile("Log mode is turned off! ",  false);
			enLog = false;
		}
		else
		{
			scripterStream->writeLogFile("Log mode is turned on! ",  false);
			enLog = true;
		}

		scripterStream->initLogTxtFile(enLog, mainPathDir);
		std::string showCommands = "";

		for (uint8_t i = 0; i < argc; i++)
		{
			showCommands += argv[i];
			showCommands += " ";
		} 
		scripterStream->writeLogFile(showCommands, false);

		/***********************************************************************************
		* Debug option  (VERBOSE)
		***********************************************************************************/
		if (vm.count("debug"))
		{
			scripterStream->writeLogFile("Verbose is turned on! ", false);
			enVerbose = true;
		}
		else
		{
			scripterStream->writeLogFile("Verbose is turned off! ", false);
			enVerbose = false;
		}

		/***********************************************************************************
		* Device option  
		***********************************************************************************/
		cliParamsVector.clear();

		if (vm.count("device"))
		{
			argCmd = vm["device"].as<std::string>();
			cliParamsVector.push_back(argCmd);
			scripterStream->writeLogFile("Device : " + argCmd, false);
  		}

		/***********************************************************************************
		* InitComm option
		***********************************************************************************/
		if (vm.count("initComm"))
		{
			argCmd = vm["initComm"].as<std::string>();
			scripterStream->writeLogFile("Init communication parameters : " + argCmd, false);
			if (util->compareString(argCmd, "USB"))
			{
				cliParamsVector.push_back(argCmd);
			}
#if defined(_WIN32)||(_WIN64)
			else if (util->compareString(argCmd.substr(0, 3), "COM"))
			{
				cliParamsVector.push_back(argCmd);
			}
#else
			else if (util->compareString(argCmd.substr(0, 3), "/dev/tty"))
			{
				cliParamsVector.push_back(argCmd);
			}
#endif
			else
			{
				util->getTokens(argCmd, &cliParamsVector);
			}

			//Call the interpreterMode, initialize the communication and family
			interpretMode(&cliParamsVector);
			callFactory();
			ptrComm->init(&modeParams);
		}

		/***********************************************************************************
		* BSL Password option
		***********************************************************************************/
		if (vm.count("bslPwd"))
		{
			argCmd = vm["bslPwd"].as<std::string>();
			commandParams.params.clear();
			if (!util->compareString(argCmd, ""))
			{
				commandParams.params.push_back(argCmd);
			}

			if (modeParams.family == Family::P4xx)
			{
				commandParams.command = Command::RX_PASSWORD_32;
				scripterStream->writeLogFile("RX_PASSWORD_32 " + argCmd, false);
			}
			else
			{
				commandParams.command = Command::RX_PASSWORD;
				scripterStream->writeLogFile("RX_PASSWORD " + argCmd, false);
			}
			executeCommand(commandParams);

			//Adding delay automatically for the Fram devices
			//  since it won't return any value when the password is incorrect
			//  It needs some time to perform mass erase inside the device
			if (modeParams.family == Family::Fram)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			}
		}

		/***********************************************************************************
		* Change Baud Rate option
		***********************************************************************************/
		if (vm.count("speed"))
		{
			argCmd = vm["speed"].as<std::string>();
			std::string baudRateString;
			if (modeParams.protocol == Protocol::Uart)
			{
				if (util->compareString(argCmd, "FAST"))
				{
					baudRateString = "115200";
				}
				else if (util->compareString(argCmd, "MEDIUM"))
				{
					baudRateString = "57600";
				}
				else if (util->compareString(argCmd, "SLOW"))
				{
					baudRateString = "9600";
				}
				else
				{
					throw std::runtime_error("Wrong parameters for change speed!");
				}
			}
			else
			{
				throw std::runtime_error("Change speed only applicable for UART!");
			}
			commandParams.params.clear();
			commandParams.params.push_back(baudRateString);
			commandParams.command = Command::CHANGE_BAUD_RATE;
			scripterStream->writeLogFile("CHANGE_BAUD_RATE " + baudRateString,  false);
			executeCommand(commandParams);
		}


		/***********************************************************************************
		* Unlock Info Memory option
		***********************************************************************************/
		if (vm.count("unlockInfo"))
		{
			commandParams.command = Command::TOGGLE_INFO;
			scripterStream->writeLogFile("TOGGLE_INFO ", false);
			executeCommand(commandParams);
		}

		/***********************************************************************************
		* Erase Memory option
		***********************************************************************************/
		bool eraseSegmentCorrelateWithWrite = false;

		if (vm.count("erase"))
		{
			argCmd = vm["erase"].as<std::string>();
			//Erase segment is only applicable for device with flash memory
			if (util->compareString(argCmd, "ERASE_SEGMENT") && ((modeParams.family == Family::Fram) || (modeParams.family == Family::Crypto)))
			{
				scripterStream->writeLogFile("[ERROR_MESSAGE]Erase segment is not applicable for FRAM devices!",true);
			}

			//Erase all and Erase Main perform the same MASS_ERASE command from BSL perspective
			if (util->compareString(argCmd, "ERASE_ALL")|| util->compareString(argCmd, "ERASE_MAIN"))
			{
				commandParams.command = Command::MASS_ERASE;
				scripterStream->writeLogFile("MASS_ERASE", false);
				executeCommand(commandParams);
			}
			//Erase segment is only applicable for device with flash memory
			else if (util->compareString(argCmd, "ERASE_SEGMENT"))
			{
				eraseSegmentCorrelateWithWrite = true;
			}
			//Default erase operation is erase_segment
			else
			{
				cliParamsVector.clear();
				util->getTokens(argCmd, &cliParamsVector);
				if (util->compareString(cliParamsVector.at(0), "ERASE_SEGMENT"))
				{
					commandParams.params.clear();
					commandParams.params.push_back(cliParamsVector.at(1));
					if (util->generalizeFamily(modeParams.family) == Family::P4xx)
					{
						commandParams.command = Command::ERASE_SEGMENT_32;
						scripterStream->writeLogFile("ERASE_SEGMENT_32 " + commandParams.params.at(0),  false);

					}
					else
					{
						commandParams.command = Command::ERASE_SEGMENT;
						scripterStream->writeLogFile("ERASE_SEGMENT " + commandParams.params.at(0),  false);
					}
					executeCommand(commandParams);
				}
				else
				{
					throw std::runtime_error("Incorrect CLI parameters for Erase command!");
				}
			}
		}
		else
		{
			if (vm.count("write"))
			{
				//The ERASE_ALL for protocol USB has to be done manually
				//  by having command: ERASE_SEGMENT. This prevents the programming 
				//  when using the RAM_USB_BSL erases the USB BSL firmware itself

				//Also for the Fram family, the mass erase will reset the device
				//  Best solution to erase the memory is first apply the wrong password to the device
				if ((modeParams.protocol != Protocol::Usb) && 
					(modeParams.family != Family::Fram) &&
					(modeParams.family != Family::E4xx))
				{
					//Default is to do the mass erase before writing memory
					commandParams.command = Command::MASS_ERASE;
					scripterStream->writeLogFile("MASS_ERASE", false);
					executeCommand(commandParams);
				}
			}
		}

		/***********************************************************************************
		* Write Memory option
		***********************************************************************************/
		if (vm.count("write"))
		{
			argCmd = vm["write"].as<std::string>();

			//Get the programmed-address, segmentsize, and crc for each programmed-segment 
			//  to be used later in memory verification
			//It is not applicable for Crypto mode, since it has different format 
			//  for the firmware image input
			if (modeParams.family != Family::Crypto)
			{
				scripterStream->initReadFile(argCmd, mainPathDir);
				scripterStream->getDataOfProgrammedMemory();
				scripterStream->closeReadFile();
			}

			//When the write is performed, the erase segment is chosen,
			//   the memory segment that will be programmed will be segment-erased.
			//   This is not applicable for Crypto and FRAM devices
			if ((eraseSegmentCorrelateWithWrite) && 
				(modeParams.family != Family::Fram) && 
				(modeParams.family != Family::Crypto) &&
				(modeParams.family != Family::E4xx))
			{
				for (uint8_t i = 0; i < scripterStream->retProgrammedMemory()->address.size(); i++)
				{
					if (modeParams.family == Family::P4xx)
					{
						commandParams.command = Command::ERASE_SEGMENT_32;
					}
					else 
					{
						commandParams.command = Command::ERASE_SEGMENT;
					}
					commandParams.params.clear();
					commandParams.params.push_back(scripterStream->retProgrammedMemory()->address.at(i));
					scripterStream->writeLogFile("ERASE_SEGMENT " + scripterStream->retProgrammedMemory()->address.at(i),  false);
					executeCommand(commandParams);
					if (ptrComm->retErrorAck() || ptrComm->retErrorStatus())
					{
						throw std::runtime_error("[ERROR_MESSAGE]Erase Segment prior to programming failed!");
					}
				}
			}
			
			//Prepare the command buffer depends on the family initialized
			commandParams.params.clear();
			commandParams.params.push_back(argCmd);
			if ((modeParams.family == Family::P4xx) || 
				((modeParams.family == Family::E4xx) && (modeParams.protocol != Protocol::Usb)))
			{
				commandParams.command = Command::RX_DATA_BLOCK_32;
				scripterStream->writeLogFile("RX_DATA_BLOCK_32 " + commandParams.params.at(0), false);
			}
			else if (modeParams.family == Family::Crypto)
			{
				commandParams.command = Command::RX_SECURE_DATA_BLOCK;
				scripterStream->writeLogFile("RX_SECURE_DATA_BLOCK " + commandParams.params.at(0),  false);
			}
			else if (modeParams.protocol == Protocol::Usb)
			{
				commandParams.command = Command::RX_DATA_BLOCK_FAST;
				scripterStream->writeLogFile("RX_DATA_BLOCK_FAST " + commandParams.params.at(0), false);
			}
			else
			{
				commandParams.command = Command::RX_DATA_BLOCK;
				scripterStream->writeLogFile("RX_DATA_BLOCK " + commandParams.params.at(0),  false);
			}
			executeCommand(commandParams);
		}


		/***********************************************************************************
		* Read memory option
		***********************************************************************************/
		if (vm.count("read"))
		{
			argCmd = vm["read"].as<std::string>();
			cliParamsVector.clear();
			util->getTokens(argCmd, &cliParamsVector);

			uint32_t addressStart = util->convertHexStringToUlong(cliParamsVector.at(1));
			uint32_t addressEnd = util->convertHexStringToUlong(cliParamsVector.at(2));
			uint32_t length = addressEnd - addressStart;

			commandParams.params.clear();
			commandParams.params.push_back(util->convertUlongToString(addressStart, true));
			commandParams.params.push_back(util->convertUlongToString(length, true));
			commandParams.params.push_back(cliParamsVector.at(0));

			//Prepare the command buffer depends on the family initialized
			if (modeParams.family == Family::P4xx)
			{
				commandParams.command = Command::TX_DATA_BLOCK_32;
				scripterStream->writeLogFile("TX_DATA_BLOCK_32 " + commandParams.params.at(0) + " " + commandParams.params.at(1) + " " + cliParamsVector.at(0), false);
			}
			else
			{
				commandParams.command = Command::TX_DATA_BLOCK;
				scripterStream->writeLogFile("TX_DATA_BLOCK " + commandParams.params.at(0) + " " + commandParams.params.at(1) + " " + cliParamsVector.at(0), false);
			}
			executeCommand(commandParams);
		}

		/***********************************************************************************
		* Verify memory option
		***********************************************************************************/
		if (vm.count("verify"))
		{
			scripterStream->writeLogFile("MEMORY VERIFICATION", false);

			bool verifyFromFile = true;


			if (!vm.count("write"))
			{
				if (modeParams.family == Family::Crypto)
				{
					throw std::runtime_error("[ERROR_MESSAGE]Verification memory for Crypto BSL is not applicable!");
				}

				argCmd = vm["verify"].as<std::string>();
				if (util->compareString(argCmd, ""))
				{
					throw std::runtime_error("[ERROR_MESSAGE]Verification without programming requires input file, or by CRC check mode!");
				}

				//Verify memory with CRC check based on the getProgrammedMemory that performed before
				if (util->compareString(argCmd.substr(0, 1), "["))
				{
					verifyFromFile = false;

					boost::char_separator<char> sep(",-[]");
					boost::tokenizer<boost::char_separator<char>> tokens(argCmd, sep);

					if (modeParams.family == Family::P4xx)
					{
						commandParams.command = Command::CRC_CHECK_32;
					}
					else
					{
						commandParams.command = Command::CRC_CHECK;
					}

					std::vector<std::string> tmp;
					for (const std::string& t : tokens)
					{
						tmp.push_back(t);
					}

					if (tmp.size() >= 2)
					{
						uint32_t length = util->convertHexStringToUlong(tmp.at(1)) - util->convertHexStringToUlong(tmp.at(0));
						commandParams.params.clear();
						commandParams.params.push_back(tmp.at(0));
						commandParams.params.push_back(util->convertUlongToString(length, true));
						if (tmp.size() == 3)
						{
							uint32_t crcInput = util->convertHexStringToUlong(tmp.at(2));
							commandParams.params.push_back(util->convertUlongToString(crcInput, true));
						}
					}
					else
					{
						throw std::runtime_error("[ERROR_MESSAGE]Wrong parameters for verification by CRC Check!");
					}

					executeCommand(commandParams);
				}
				//Verify memory with input file, when the write memory option is not performed
				else
				{
					verifyFromFile = true;
					scripterStream->initReadFile(argCmd, mainPathDir);
					scripterStream->getDataOfProgrammedMemory();
					scripterStream->closeReadFile();
				}
			}


			if ((verifyFromFile) && (modeParams.family != Family::Crypto))
			{
				bool tmpEnQuiet = scripterStream->getEnQuiet();
				scripterStream->setEnQuiet(true);
				for (uint8_t i = 0; i < scripterStream->retProgrammedMemory()->address.size(); i++)
				{
					commandParams.command = Command::CRC_CHECK;
					commandParams.params.clear();
					commandParams.params.push_back(scripterStream->retProgrammedMemory()->address.at(i));
					commandParams.params.push_back(scripterStream->retProgrammedMemory()->sizeProgrammed.at(i));
					commandParams.params.push_back(scripterStream->retProgrammedMemory()->crcCheck.at(i));
					executeCommand(commandParams);

					if (!ptrComm->retCrcCheckResult())
					{
						throw std::runtime_error("[ERROR_MESSAGE]Verification memory failed!");
					}

				}
				scripterStream->setEnQuiet(tmpEnQuiet);
				scripterStream->writeLogFile("[SUCCESS_MESSAGE]Verification memory status is success!", true);
			}
		}

		/***********************************************************************************
		* Lock JTAG option
		***********************************************************************************/
		if (vm.count("lockJtag"))
		{
			jtagLockWithPwd = false;

			//Only applicable form Fram and Flash family
			if ((modeParams.family == Family::Fram) || (modeParams.family == Family::Flash))
			{
				scripterStream->writeLogFile("LOCKING JTAG", false);
				scripterStream->writeLogFile("by implementing electronic fuse", true);

				//Init the location of JTAG signature based on the family
				util->initJtagLockSignature(modeParams);

				//Locking JTAG with password is only applicable for FRAM family
				if (vm.count("pwdLockJtag"))
				{
					jtagLockWithPwd = true;
					argCmd = vm["pwdLockJtag"].as<std::string>();
					if ((modeParams.family == Family::Fram) && (!util->compareString(argCmd, "")))
					{
						boost::char_separator<char> sep(",[]");
						boost::tokenizer<boost::char_separator<char>> tokens(argCmd, sep);

						std::vector<std::string> tmp;
						for (const std::string& t : tokens)
						{
							tmp.push_back(t);
						}
						util->setJtagPasswordForFram(&tmp);
					}
				}
				commandParams.command = Command::JTAG_LOCK;
				executeCommand(commandParams);
				if (jtagLockWithPwd)
				{
					commandParams.command = Command::JTAG_PASSWORD;
					executeCommand(commandParams);
				}
			}
			else
			{
				scripterStream->writeLogFile("[ERROR_MESSAGE]This feature only applicable for Flash and Fram devices!", true);
			}
		}

		/***********************************************************************************
		* Exit option
		***********************************************************************************/
		if (vm.count("exit"))
		{
			argCmd = vm["exit"].as<std::string>();
			if (argCmd != "")
			{
				cliParamsVector.clear();
				util->getTokens(argCmd, &cliParamsVector);
				if (util->compareString(cliParamsVector.at(0), "SET_PC"))
				{
					commandParams.command = Command::SET_PC;
					commandParams.params.at(0) = cliParamsVector.at(1);
					scripterStream->writeLogFile("SET_PC " + commandParams.params.at(0), false);
				}
				else if (util->compareString(cliParamsVector.at(0), "SET_PC_32"))
				{
					commandParams.command = Command::SET_PC_32;
					commandParams.params.at(0) = cliParamsVector.at(1);
					scripterStream->writeLogFile("SET_PC_32 " + commandParams.params.at(0), false);
				}
				else if (util->compareString(cliParamsVector.at(0), "RESET"))
				{
					commandParams.command = Command::REBOOT_RESET;
					scripterStream->writeLogFile("REBOOT_RESET ", false);
				}
				executeCommand(commandParams);
			}
			ptrComm->close();
			scripterStream->closeLogFile();
			return 0;
		}
		boost::program_options::notify(vm);
	}
	catch (boost::program_options::error& e)
	{
		std::cerr << "Error in : " << e.what() << std::endl << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}
	return 0;
}

void Interpreter::mainControllerScript()
{
	//Main controller Script is a loop that reads all lines in the script file
	while (scripterStream->getLine())
	{
		std::string line = scripterStream->retStringCommand();

		//Writes the command or comments on the script file to the log file
		scripterStream->writeLogFile(line, false);

		//Interpret the line when it is not a comment line
		if (!util->isUnexecutedLine(line))
		{
			matchCommand(scripterStream->retStringCommand());
			lineNum++;

			if (lineNum == 1)
			{
				if (commandParams.command == Command::LOG)
				{
					enLog = true;
					lineNum = 0;
					scripterStream->initLogTxtFile(enLog, mainPathDir);
				}
				else if (commandParams.command == Command::MODE)
				{
					interpretMode(&commandParams.params);
					callFactory();
					ptrComm->init(&modeParams);
				}
				else
				{
					throw std::runtime_error("Please configure the MODE command before starting the other commands!");
				}
			}
			else if ((lineNum > 1) && (commandParams.command == Command::MODE))
			{
				ptrComm->close();
				interpretMode(&commandParams.params);
				callFactory();
				ptrComm->init(&modeParams);
			}
			else
			{
				executeCommand(commandParams);
			}
		}
	}
	scripterStream->closeScriptFile();
	ptrComm->close();
}

void Interpreter::matchCommand(std::string line)
{
	lineTokens.clear();
	lineTokens = util->getTokens(line);

	if (!(util->checkExistence(lineTokens.at(0), util->ptrCommandsVector())))
	{
		throw std::runtime_error("Command is invalid!");
	}
	else
	{
		commandParams.command = (Command)util->retNumToEnum();
		commandParams.params.clear();
		for (uint8_t i = 1; i < lineTokens.size(); i++)
		{
			commandParams.params.push_back(lineTokens.at(i));
		}
	}
}

void Interpreter::interpretMode(std::vector<std::string> *params)
{
	bool familyPresent = false;
	bool protocolPresent = false;
	bool comPortPresent = false;
	bool macAddressPresent = false;
	bool clientIpAddrPresent = false;

	bool uartBaudRatePresent = false;
	bool i2cBaudRatePresent = false;
	bool spiBaudRatePresent = false;

	uint32_t baudRate = 0;
	std::string param;

	modeParams.crypto = false;
	modeParams.enParity = false;
	modeParams.enInvoke = false;
	modeParams.useMspFet = false;
	modeParams.disableAutoBaud = false;
	modeParams.i2cSlaveAddr = 0;

	modeParams.clientIpAddress = "";
	modeParams.serverIpAddress = "0.0.0.0";

	//Check the all parameters independently
	for (uint8_t i = 0; i < (*params).size(); i++)
	{
		param = (*params).at(i);
		//Check the family
		if (util->checkExistence(param, util->ptrFamilyVector()))
		{
			modeParams.family = (Family)util->retNumToEnum();
			familyPresent = true;
		}
		//Check the protocol
		if (util->checkExistence(param, util->ptrProtocolVector()))
		{
			numToEnum = util->retNumToEnum();
			modeParams.protocol = (Protocol)util->retNumToEnum();
			protocolPresent = true;
		}

		//Check the comport
#if defined (_WIN32) || (_WIN64)
		if (util->checkExistence(param, "COM", 3))
		{
			modeParams.commPort = param;
			comPortPresent = true;
		}
#else
		if (util->checkExistence(param, "/dev/tty", 8))
		{
			modeParams.commPort = param;
			comPortPresent = true;
		}
#endif
		//Check baudrate
		if (util->checkExistence(param, util->ptrUartBaudRateVector()))
		{
			baudRate = std::stoul(param, NULL, 0);
			uartBaudRatePresent = true;
		}

		if (util->checkExistence(param, util->ptrI2cBaudRateVector()))
		{
			baudRate = std::stoul(param, NULL, 0);
			i2cBaudRatePresent = true;
		}

		if (util->checkExistence(param, util->ptrSpiBaudRateVector()))
		{
			baudRate = std::stoul(param, NULL, 0);
			spiBaudRatePresent = true;
		}

		//Check the Crypto
		if (util->compareString(param, CRYPTO_ARG))
		{
			modeParams.crypto = true;
		}

		//Check the parity
		if (util->compareString(param, PARITY_ARG))
		{
			modeParams.enParity = true;
		}

		if (util->compareString(param, INVOKE_ARG)) {
			modeParams.enInvoke = true;
		}

		//Check the parity
		if (util->compareString(param, MSPFET1_ARG) || util->compareString(param, MSPFET2_ARG))
		{
			modeParams.useMspFet = true;
		}

		//Check if the AutoBaud shall be disabled
		if (util->compareString(param, DISABLE_AUTO_BAUD) )
		{
			modeParams.disableAutoBaud = true;
		}

		//Check if there is MAC address 
		if (util->checkExistence(param, MAC_ARG, MAC_ARG.size()))
		{
			macAddressPresent = true;
			//get the MAC address and erase the "MAC=" string
			modeParams.macAddress = param.substr(MAC_ARG.size(), param.size());
		}

		//Check if there is and IP Client Address available
		if (util->checkExistence(param, IP_CLIENT_ARG, IP_CLIENT_ARG.size()))
		{
			//get the IP address and erase the "IPClient=" string
			modeParams.clientIpAddress = param.substr(IP_CLIENT_ARG.size(), param.size());
			clientIpAddrPresent = true;
		}

		//Check if there is and IP Server Address available
		if (util->checkExistence(param, IP_SERVER_ARG, IP_SERVER_ARG.size()))
		{
			//get the IP address and erase the "IPServer=" string
			modeParams.serverIpAddress = param.substr(IP_SERVER_ARG.size(), param.size());
		}

		//Check if there is custom I2C slave address
		if (util->checkExistence(param, I2C_ADDRESS, I2C_ADDRESS.size()))
		{
			//get the IP address and erase the "I2CAddr=" string
			modeParams.i2cSlaveAddr = util->convertHexStringToUlong(param.substr(I2C_ADDRESS.size(), param.size()));
		}
	}

	//Throw exception if the family is not present
	if (!familyPresent)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]Family of the target device is not identified!");
	}

	//If protocol is not present, the UART would be the default
	if (!protocolPresent)
	{
		scripterStream->writeLogFile("\t[ERROR_MESSAGE]Protocol is set to the default: UART", false);
		modeParams.protocol = Protocol::Uart;
	}

	//Once family is obtained, check if protocol is valid
	//  since the family could be declared in different names, we generalize it 
	//  to make the comparison to the family easier
	modeParams.family = util->generalizeFamily(modeParams.family);
	if (!util->combinationFamilyProtocol(modeParams.family, modeParams.protocol))
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]The communication protocol chosen is not available for the chosen Family!");
	}

	//Check Crypto is valid for FRAM devices
	if (((modeParams.family == Family::Fram) && (modeParams.crypto == true)) ||
		((modeParams.family == Family::Crypto) && (modeParams.crypto == true)))
	{
		modeParams.family = Family::Crypto;
	}
	else if (((modeParams.family != Family::Fram) && (modeParams.crypto == true)) ||
		((modeParams.family != Family::Crypto) && (modeParams.crypto == true)))
	{
		throw std::runtime_error("[ERROR_MESSAGE]CryptoBSL is not applicable except for FRAM devices!");
	}

	//Check baudrate
	modeParams.baudRate = util->checkBaudRate(modeParams.protocol, baudRate,
		uartBaudRatePresent, i2cBaudRatePresent, spiBaudRatePresent);

	//Check COMPORT
	if (!comPortPresent && ((modeParams.protocol == Protocol::I2c) ||
		(modeParams.protocol == Protocol::Uart) ||
		(modeParams.protocol == Protocol::Spi)))
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]Please define the COM PORT!");
	}

	//Check the parameters for Ethernet protocol
	if (modeParams.protocol == Protocol::Ethernet)
	{
		if (macAddressPresent && clientIpAddrPresent)
		{
			//Validate the MAC address and Client IP Address
			if (!util->isEthernetParamsValid(modeParams.macAddress, modeParams.clientIpAddress, modeParams.serverIpAddress))
			{
				throw std::runtime_error("\t[ERROR_MESSAGE]Wrong given MAC address or IP address format!");
			}
		}
		else
		{
			throw std::runtime_error("\t[ERROR_MESSAGE]MAC address and Client IP parameters are missing!");
		}
	}

	//Init defaultPass
	if (modeParams.family == Family::F543x)
	{
		util->initDefaultPassword(PasswordSize::PWD_16_BIT);
	}
	else if (modeParams.family == Family::P4xx)
	{
		util->initDefaultPassword(PasswordSize::PWD_256_BIT);
	}
	else
	{
		util->initDefaultPassword(PasswordSize::PWD_32_BIT);
	}

	//Assign buffer properties for scripterStream properties
	//the default is the MSP430 serial 
	modeParams.blockDataSize = BlockDataSize::MSP430_MSP432P_SERIAL;
	modeParams.headerSize = HeaderSize::MSP430_MSP432P;
	modeParams.checksumSize = CheckSumSize::MSP430_MSP432P;
	modeParams.segmentSize = SegmentSize::MSP430;

	switch (modeParams.family)
	{
	case(Family::E4xx) :
		modeParams.segmentSize = SegmentSize::MSP432E;
		if (modeParams.protocol == Protocol::Usb)
		{
			modeParams.checksumSize = CheckSumSize::MSP432E_USBDFU;
			modeParams.headerSize = CheckSumSize::MSP432E_USBDFU;
		}
		else
		{
			modeParams.checksumSize = CheckSumSize::MSP432E;
			modeParams.headerSize = HeaderSize::MSP432E;
		}
		if (modeParams.protocol == Protocol::Ethernet)
		{
			modeParams.blockDataSize = BlockDataSize::MSP432E_ETHERNET;
		}
		else if (modeParams.protocol == Protocol::Usb)
		{
			modeParams.blockDataSize = BlockDataSize::MSP432E_USBDFU;
		}
		else
		{
			modeParams.blockDataSize = BlockDataSize::MSP432E_SERIAL;
		}
		break;

	case(Family::P4xx) :
		modeParams.segmentSize = SegmentSize::MSP432P;
		break;

	default: 
		//Applicable for MSP430 families with USB protocol
		if (modeParams.protocol == Protocol::Usb)
		{
			modeParams.blockDataSize = BlockDataSize::MSP430_USB;
			modeParams.headerSize = HeaderSize::MSP430_USB;
			modeParams.checksumSize = CheckSumSize::MSP430_USB;
		}
		break;
	}
 	scripterStream->setBufferProperties(modeParams.blockDataSize, modeParams.headerSize, modeParams.checksumSize, modeParams.segmentSize);
}

void Interpreter::callFactory()
{
	ptrTarget = factoryTarget.set(&modeParams, util);
	ptrComm = factoryCommunication.set(&modeParams, util);
}

void Interpreter::executeCommand(CommandParams cmd)
{
	ptrComm->resetIdxAcc();
	if (util->isPcCommand(commandParams.command))
	{
		executePcCommand();
	}
	else if (util->isRxCommand(commandParams.command))
	{
		executeRxCommand();
	}
	else if (util->isTxCommand(commandParams.command))
	{
		if (modeParams.family == Family::E4xx)
		{
			executeTxCommandE4xx();
		}
		else
		{
			executeTxCommand();
		}
	}
	else
	{
		if (modeParams.family == Family::E4xx)
		{
			executeOtherCommandE4xx();
		}
		else
		{
			executeOtherCommand();
		}
	}
}

void Interpreter::executeRxCommand()
{
	std::string statusExecution;
	ptrComm->resetBytesWritten();

	//Reset the number of bytes	sent to start new loop of transmission
	ptrComm->resetBytesSent();

	if (commandParams.command == Command::JTAG_LOCK)
	{
		executeJtagLock();
	}
	else if (commandParams.command == Command::JTAG_PASSWORD)
	{
		executeJtagPasswordSetup();
	}
	else if (((commandParams.command == Command::RX_PASSWORD) || (commandParams.command == Command::RX_PASSWORD_32))
		      && (commandParams.params.size() == 0))
	{
		executeDefaultPasswordCommand();
	}
	else if (commandParams.command == Command::CUSTOM_COMMAND)
	{
		executeCustomCommand();
	}
	//Execute RX_DATA_BLOCK_FAST for USB_DFU MSP432E
	else if ((commandParams.command == Command::RX_DATA_BLOCK_FAST) &&
		     (modeParams.family == Family::E4xx))
	{
		timer.resetTimeCount();
		timer.startUpTimer();
		ptrComm->resetBytesWritten();
		scripterStream->resetIntelHex03detect();
		scripterStream->initReadFile(commandParams.params[0], mainPathDir);

		while (scripterStream->retScripterReadStatus() == ScripterReadStatus::OnGoing)
		{
			if (scripterStream->isTxtFile())
			{
				scripterStream->readTxtFileE4xxUsbDfu();
			}
			else
			{
				scripterStream->readHexFileE4xxUsbDfu();
			}
			
			// A data block is ready
			if (scripterStream->retScripterReadStatus() == ScripterReadStatus::DataBlockReady)
			{
				CommandParams E4xxDownload;
				E4xxDownload.command = Command::DOWNLOAD;
				ptrTarget->selectRxCommand(&E4xxDownload, scripterStream->retDataBlockRead());
				ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
				scripterStream->showVerboseE4xxUsbDfu(ptrTarget->retPtrCommandBuffer(), ptrComm->retPtrRxBuffer(), Msp432ECommandSize::SIZE, 0, this->enVerbose);

				//Send package with no loop, as the maximum package size is 1024 and has to be followed by 
				//   payload length 0
				ptrTarget->selectRxCommand(&commandParams, scripterStream->retDataBlockRead());
				ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
				ptrComm->receiveBuffer(0);
				scripterStream->showVerboseE4xxUsbDfu(ptrTarget->retPtrCommandBuffer(), ptrComm->retPtrRxBuffer(), scripterStream->retDataBlockRead()->data.size(), UsbDfuResponseSize::GET_STATUS, this->enVerbose);
				scripterStream->clearDataBlock();
			}
			if (scripterStream->retIntelHex03detect()) 
			{
				scripterStream->writeLogFile("SET_PC command is not available in USB DFU mode",true);
			}

			if (scripterStream->retReadLineStatus() == ReadLineStatus::QuitSign)
			{
				break;
			}
		}
		elapsedTime = timer.returnTimeCount();
		scripterStream->calcTransSpeed(elapsedTime, ptrComm->retBytesWritten(), scripterStream->READ_MODE);
		ptrComm->resetBytesWritten();
	}
	//Execute RX_DATA_BLOCK, RX_DATA_BLOCK_32, RX_DATA_BLOCK_FAST, and RX_DATA_BLOCK_SECURE
	else
	{
		timer.resetTimeCount();
		timer.startUpTimer();
		scripterStream->resetIntelHex03detect();
		scripterStream->initReadFile(commandParams.params[0], mainPathDir);

		if (commandParams.command == Command::RX_DATA_BLOCK_FAST && modeParams.protocol!=Protocol::Usb)
		{
			scripterStream->setBufferProperties((uint16_t)BlockDataSize::MSP430_USB,
												(uint16_t)HeaderSize::MSP430_USB,
												(uint16_t)CheckSumSize::MSP430_USB,
												modeParams.segmentSize);
		}

		while (scripterStream->retScripterReadStatus() == ScripterReadStatus::OnGoing)
		{
			//Reading data from the firmware image
			if (scripterStream->isTxtFile())
			{
				if (modeParams.family == Family::E4xx)
				{
					if (modeParams.protocol == Protocol::Ethernet)
					{
						scripterStream->readTxtFileE4xxEthernet();
					}
					else
					{
						scripterStream->readTxtFileE4xx();
					}
				}
				else
				{
					scripterStream->readTxtFile();
				}
			}
			else
			{
				if (modeParams.family == Family::E4xx)
				{
					if (modeParams.protocol == Protocol::Ethernet)
					{
						scripterStream->readHexFileE4xxEthernet();
					}
					else
					{
						scripterStream->readHexFileE4xx();
					}
				}
				else
				{
					scripterStream->readHexFile();
				}
			}

			if (scripterStream->retScripterReadStatus() == ScripterReadStatus::DataBlockReady)
			{
				//The transmission is executed the the dataBlockRead is not empty
				if (scripterStream->retDataBlockRead()->data.size() != 0)
				{
					//Only for MSP432E4xx the first command would be DOWNLOAD command to give the 
					//address and size of firmware image parameters
					if (modeParams.family == Family::E4xx) 
					{
						CommandParams E4xxDownload;
						E4xxDownload.command = Command::DOWNLOAD;
						ptrTarget->selectRxCommand(&E4xxDownload, scripterStream->retDataBlockRead());
						if (modeParams.protocol != Protocol::Ethernet)
						{
							this->getNumOfResponse();
							ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
							ptrComm->receiveBuffer(numOfResponse);
							scripterStream->showVerboseE4xx(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), this->enVerbose);
						}
					}

					while (ptrComm->retBytesSent() < scripterStream->retDataBlockRead()->data.size())
					{
						ptrTarget->selectRxCommand(&commandParams, scripterStream->retDataBlockRead());
						this->getNumOfResponse();
						ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
						ptrComm->receiveBuffer(numOfResponse);

						if (modeParams.family == Family::E4xx)
						{
							scripterStream->showVerboseE4xx(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), this->enVerbose);
						}
						else
						{
							scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), 0, ptrComm->retLastRxBufferIdx(), this->enVerbose);
						}
 
						//Check if writing is okay and if it is not RX_DATA_BLOCK_FAST
						if (commandParams.command != Command::RX_DATA_BLOCK_FAST)
						{
							ptrComm->processResponse(ptrComm->retAck(), ptrComm->retPtrRxBuffer());
							if (ptrComm->retErrorAck())
							{
								statusExecution = ptrComm->retErrorAckMessage();
								scripterStream->writeLogFile(statusExecution, true);
								ptrComm->resetIdxAcc();
								break;
							}
							else
							{
								if (ptrComm->retErrorStatus())
								{
									statusExecution = ptrComm->retErrorStatusMessage();
									scripterStream->writeLogFile(statusExecution, true);
									ptrComm->resetIdxAcc();
									break;
								}
							} 
						}
					}
					scripterStream->clearDataBlock();
					ptrComm->resetBytesSent();

					//Execute the SET_PC or SET_PC_32 when the Intel Hex "0x03" is read
					if ((scripterStream->retIntelHex03detect()) && (modeParams.protocol != Protocol::Ethernet))
					{
						//execute the set pc
						if ((modeParams.family == Family::MSP432P4xx) || (modeParams.family == Family::E4xx))
						{
							commandParams.command = Command::SET_PC_32;
						}
						else
						{
							commandParams.command = Command::SET_PC;
						}
						commandParams.params.at(0) = scripterStream->retPcAddressIntelHex();
						executeCommand(commandParams);
						scripterStream->resetIntelHex03detect();
						scripterStream->closeReadFile();
						break;
					}
					if (scripterStream->retReadLineStatus() == ReadLineStatus::QuitSign)
					{
						break;
					}
				}
			}
			ptrComm->resetIdxAcc();
		}
		scripterStream->closeReadFile();

		if ((commandParams.command != Command::RX_PASSWORD) &&
			(commandParams.command != Command::RX_PASSWORD_32))
		{
			elapsedTime = timer.returnTimeCount();
			scripterStream->calcTransSpeed(elapsedTime, ptrComm->retBytesWritten(), scripterStream->READ_MODE);
			ptrComm->resetBytesWritten();
		}
		else
		{
			if (!ptrComm->retErrorStatus())
			{
				this->getAndTranslateResponse();
			}
		}
	}
	ptrComm->resetIdxAcc();
}

void Interpreter::executeTxCommand()
{
	if (modeParams.protocol == Protocol::Usb && scripterStream->isHexFile())
	{
		scripterStream->setBufferProperties(32, 3, 0, 512);
	}
	else  if (modeParams.protocol == Protocol::Usb && scripterStream->isTxtFile())
	{
		scripterStream->setBufferProperties(48, 3, 0, 512);
	}

	timer.resetTimeCount();
	timer.startUpTimer();

	uint32_t address = util->convertHexStringToUlong(commandParams.params.at(0));
	uint32_t totalByte = util->convertHexStringToUlong(commandParams.params.at(1));

	uint32_t reqAddress = address;
	uint32_t reqTotalByte;
	uint32_t maxByte = scripterStream->retNumOfDataPerPacket();
	uint32_t idx = 0;
	uint32_t loop = 0;
	uint32_t cntByte = 0;
	std::string statusExecution = "";

	if (totalByte <= 0xFFFF)
	{
		//initFile
		scripterStream->initWriteFile(commandParams.params.at(2), commandParams.params.at(0), mainPathDir);

		if (totalByte > maxByte)
		{
			reqTotalByte = maxByte;
		}
		else
		{
			reqTotalByte = totalByte;
		}
		while (ptrComm->retBytesRead() < totalByte)
		{
			//sendcommand
			ptrTarget->selectTxCommand(&commandParams, reqAddress, reqTotalByte);
			//receivebytes
			ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
			ptrComm->receiveBuffer(reqTotalByte);
			//Check if reading is okay 
			ptrComm->processResponse(ptrComm->retAck(), ptrComm->retPtrRxBuffer());
			if (ptrComm->retErrorAck())
			{
				statusExecution = ptrComm->retErrorAckMessage();
				scripterStream->writeLogFile(statusExecution, true);
				break;
			}
			else
			{
				if (ptrComm->retErrorStatus())
				{
					statusExecution = ptrComm->retErrorStatusMessage();
					scripterStream->writeLogFile(statusExecution, true);
					break;
				}
			}

			if (loop == 0)
			{
				idx = 0;
			}
			else
			{
				idx = ptrComm->retLastRxBufferIdx() - scripterStream->retNumberOfCheckSum() - scripterStream->retNumOfDataPerPacket() - scripterStream->retNumOfHeader();
			}
			cntByte = reqTotalByte + scripterStream->retNumberOfCheckSum() + scripterStream->retNumOfHeader();
			scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), idx, cntByte, this->enVerbose);

			reqAddress += reqTotalByte;

			if ((totalByte - ptrComm->retBytesRead()) > maxByte)
			{
				reqTotalByte = maxByte;
			}
			else
			{
				reqTotalByte = totalByte - ptrComm->retBytesRead();
			}
			loop++;
		}
		//write to file
		if (scripterStream->isTxtFile())
		{
			scripterStream->writeTxtFile(ptrComm->retPtrRxBuffer(), ptrComm->retLastRxBufferIdx());
		}
		else
		{
			scripterStream->writeHexFile(ptrComm->retPtrRxBuffer(), ptrComm->retLastRxBufferIdx());
		}
		scripterStream->closeWriteFile();

		elapsedTime = timer.returnTimeCount();
		scripterStream->calcTransSpeed(elapsedTime, ptrComm->retBytesRead(), scripterStream->WRITE_MODE);
	}
	else
	{
		scripterStream->writeLogFile("[ERROR_MESSAGE]Maximum number of bytes to be read is 0xFFFF!",true);
	}
}

void Interpreter::executeTxCommandE4xx()
{
	timer.resetTimeCount();
	timer.startUpTimer();

	uint32_t address = util->convertHexStringToUlong(commandParams.params.at(0));
	uint32_t totalByte = util->convertHexStringToUlong(commandParams.params.at(1));
	uint32_t reqTotalByte;
	uint32_t maxByte = scripterStream->retNumOfDataPerPacket();

	ptrComm->resetIdxAcc();

	if (totalByte <= ProgramMemorySize::MSP432E)
	{
		CommandParams disableBinFileCommand;
		disableBinFileCommand.command = Command::DISABLE_BIN_FILE;
		ptrTarget->selectTxCommand(&disableBinFileCommand, address, totalByte);
		ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());

		// Send command to read the memory with parameters of
		//    start address
		//    how many bytes would like to be read
		ptrTarget->selectTxCommand(&commandParams, address, totalByte);
		ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());

		// Init Fileto write the read data
		scripterStream->initWriteFile(commandParams.params.at(2), commandParams.params.at(0), mainPathDir);

		if (totalByte > maxByte)
		{
			reqTotalByte = maxByte;
		}
		else
		{
			reqTotalByte = totalByte;
		}

		while (ptrComm->retBytesRead() < totalByte)
		{
			//receivebytes
			ptrComm->receiveBuffer(reqTotalByte);

			//write to file
			if (scripterStream->isTxtFile())
			{
				scripterStream->writeTxtFile(ptrComm->retPtrRxBuffer(), reqTotalByte);
			}
			else
			{
				scripterStream->writeHexFile(ptrComm->retPtrRxBuffer(), reqTotalByte);
			}

			if ((totalByte - ptrComm->retBytesRead()) > maxByte)
			{
				reqTotalByte = maxByte;
			}
			else
			{
				reqTotalByte = totalByte - ptrComm->retBytesRead();
			}
		}

		scripterStream->closeWriteFile();

		elapsedTime = timer.returnTimeCount();
		scripterStream->calcTransSpeed(elapsedTime, ptrComm->retBytesRead(), scripterStream->WRITE_MODE);
	}
	else
	{
		scripterStream->writeLogFile("[ERROR_MESSAGE]Maximum number of bytes exceeds the memory size of MSP432E!", true);
	}
}

void Interpreter::executePcCommand()
{
	uint32_t delay = 0;

	switch (commandParams.command)
	{
		case(Command::DELAY) :
			delay = util->convertNumStringToUlong(commandParams.params.at(0));
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
			scripterStream->writeLogFile(("Delay " + commandParams.params.at(0) + " ms"), true);
			break;

		case(Command::VERBOSE) :
			if (enVerbose)
			{
				enVerbose = false;
				scripterStream->writeLogFile("Verbose mode is now off!", true);
			}
			else
			{
				enVerbose = true;
				scripterStream->writeLogFile("Verbose mode is now on!", true);
			}
			 break;

		case(Command::APP_VERSION) :
			executeAppVersion();
			break;

		default:
			break;
	}
}

void Interpreter::executeOtherCommand()
{
	std::string statusExecution;
	std::vector<uint8_t> tmpAck; 
	this->getNumOfResponse();
	ptrTarget->selectCommand(&commandParams);
	ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
	if (numOfResponse > 0)
	{
		ptrComm->receiveBuffer(numOfResponse);
		if (commandParams.command == Command::CHANGE_BAUD_RATE)
		{
			tmpAck.push_back(ptrComm->retAck());
			scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), &tmpAck, 0, 1, this->enVerbose);
		}
		else
		{
			scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), 0, ptrComm->retLastRxBufferIdx(), this->enVerbose);
		}
		this->getAndTranslateResponse();
	}
	else
	{
		scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), &tmpAck, 0, 0, this->enVerbose);
	}

	if (commandParams.command == Command::CHANGE_BAUD_RATE)
	{
		ptrComm->changeBaudRate(util->convertNumStringToUlong(commandParams.params.at(0)));
	}
}
  
void Interpreter::executeOtherCommandE4xx()
{
	ptrComm->resetBytesWritten();
	ptrTarget->selectCommand(&commandParams);
	ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
	// In REBOOT_RESET command, the USB DFU does not set the response bytes
	if (commandParams.command != Command::REBOOT_RESET)
	{
		ptrComm->receiveBuffer(UsbDfuResponseSize::GET_STATUS);
	}
	scripterStream->showVerboseE4xxUsbDfu(ptrTarget->retPtrCommandBuffer(), ptrComm->retPtrRxBuffer(), Msp432ECommandSize::SIZE, UsbDfuResponseSize::GET_STATUS, this->enVerbose);
}

void Interpreter::getNumOfResponse()
{
	//In USB protocol and MSP432E4xx family, the number of bytes of response
	//   are fix
	numOfResponse = ptrTarget->retNumOfResponse(&commandParams);
	if ((modeParams.protocol != Protocol::Usb) && (modeParams.family != Family::E4xx))
	{
		if (commandParams.command == Command::CHANGE_BAUD_RATE)
		{
			numOfResponse = -1;
		}
		else if (numOfResponse != 0)
		{
			numOfResponse = 1;
		}
	}
}

void Interpreter::getAndTranslateResponse()
{
	std::string statusExecution="";
	ptrComm->processResponse(ptrComm->retAck(), ptrComm->retPtrRxBuffer());
	if (commandParams.command == Command::CHANGE_BAUD_RATE)
	{		 
		if (ptrComm->retAck() == 0x00)
		{
			scripterStream->writeLogFile("[SUCCESS_MESSAGE]Baud rate change is successful!", true);
		}
	}
	else if (ptrComm->retErrorAck())
	{
		statusExecution = ptrComm->retErrorAckMessage();
		scripterStream->writeLogFile(statusExecution, true);
	}
	else
	{
		if (ptrComm->retErrorStatus())
		{
			statusExecution = ptrComm->retErrorStatusMessage();
			scripterStream->writeLogFile(statusExecution, true);
		}
		else
		{
			if (modeParams.protocol == Protocol::Usb)
			{
				statusExecution = ptrComm->translateResponse(&commandParams, ptrComm->retPtrRxBuffer(), 3);
			}
			else
			{
				statusExecution = ptrComm->translateResponse(&commandParams, ptrComm->retPtrRxBuffer(), 4);
			}
			scripterStream->writeLogFile((statusExecution), true);
		}
	}
}

std::string Interpreter::exitSpecsInfo()
{
	std::string tmp;
	tmp  = "Exit Specification Parameters\n";
	tmp += "Options:\n";
	tmp += "  N/A                exit the scripter without parameters\n";
	tmp += "  [SET_PC,<addr>]    Load the <addr> in 16bit Family devices\n";
	tmp += "  [SET_PC_32,<addr>] Load the <addr> in 32bit Family devices \n";
	tmp += "  [RESET]            Apply soft-reset to the device\n";
	tmp += "\n";
	return tmp;
}

void Interpreter::executeJtagLock()
{
	ptrTarget->selectRxCommand(&commandParams, util->retPtrJtagLockSignature());
	this->getNumOfResponse();
	ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
	ptrComm->receiveBuffer(numOfResponse);
	scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), 0, ptrComm->retLastRxBufferIdx(), this->enVerbose);
	this->getAndTranslateResponse();
}

void Interpreter::executeJtagPasswordSetup()
{
	ptrTarget->selectRxCommand(&commandParams, util->retPtrJtagLockPassword());
	this->getNumOfResponse();
	ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
	ptrComm->receiveBuffer(numOfResponse);
	scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), 0, ptrComm->retLastRxBufferIdx(), this->enVerbose);
	this->getAndTranslateResponse();
}

void Interpreter::executeDefaultPasswordCommand()
{
	ptrTarget->selectRxCommand(&commandParams, util->ptrDefaultPass());
	this->getNumOfResponse();
	ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
	ptrComm->receiveBuffer(numOfResponse);
	scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), 0, ptrComm->retLastRxBufferIdx(), this->enVerbose);
	this->getAndTranslateResponse();
}

void Interpreter::executeCustomCommand()
{
	scripterStream->initReadCustomCommand(commandParams.params[0], mainPathDir);
	while (scripterStream->retScripterReadStatus() == ScripterReadStatus::OnGoing)
	{
		scripterStream->readCustomCommand();
		if (scripterStream->retScripterReadStatus() == ScripterReadStatus::DataBlockReady)
		{
			ptrTarget->selectCustomCommand(&commandParams, scripterStream->retPtrCustomCmdBuffer());
			this->getNumOfResponse();
			ptrComm->transmitBuffer(ptrTarget->retPtrCommandBuffer());
			ptrComm->receiveBuffer(numOfResponse);
			scripterStream->clearDataBlock();
			scripterStream->showVerbose(ptrComm->retPtrTxBuffer(), ptrComm->retPtrRxBuffer(), 0, ptrComm->retLastRxBufferIdx(), this->enVerbose);
			//translate the response
			this->getAndTranslateResponse();
			if (scripterStream->retReadLineStatus() == ReadLineStatus::QuitSign)
			{
				break;
			}
		}
	}
	scripterStream->closeReadFile();
}

void Interpreter::executeAppVersion()
{
	std::string version = std::to_string(Version::MAJOR) + "." +
						  std::to_string(Version::MINOR) + "." +
						  std::to_string(Version::BUG) + "." +
						  std::to_string(Version::BUILD);

	scripterStream->writeLogFile(version, true);
}
  