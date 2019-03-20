/*
==Util.cpp==

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

const uint8_t Util::MAC_NUM_OF_BYTES;

Util::Util()
{
	initCommands();
	initFamilyEnum();
	initProtocolEnum();
	initUartBaudRateEnum();
	initI2cBaudRateEnum();
	initSpiBaudRateEnum();
}

Util::~Util()
{

}
   
void Util::initDefaultPassword(uint16_t size)
{
	defaultPass.startAddress = 0;
	defaultPass.data.clear();
	for (uint16_t i = 0; i < size; i++)
	{
		defaultPass.data.push_back(0xFF);
	}
}

void Util::initJtagLockSignature(ModeParams modeParams)
{
	jtagLockSignature.data.clear();
	if (modeParams.family == Family::Fram)
	{											
		jtagLockSignature.startAddress = 0x0000FF80;
	}
	else if (modeParams.family == Family::Flash)
	{
		jtagLockSignature.startAddress = 0x000017FC;
	}

	for (uint16_t i = 4; i--;)
	{
		jtagLockSignature.data.push_back(0x55);
	}
}

DataBlock* Util::retPtrJtagLockPassword()
{
	return &jtagLockPassword;
}

void Util::setJtagPasswordForFram(std::vector<std::string> *pwd)
{
	//Different signature for locking JTAG with password
	jtagLockSignature.data.clear();
	for (uint16_t i = 4; i--;)
	{
		jtagLockSignature.data.push_back(0xAA);
	}

	jtagLockPassword.startAddress = 0x0000FF88;
	jtagLockPassword.data.clear();
	uint32_t pwdLength = convertHexStringToUlong(pwd->at(0));
	if (pwdLength > 0xFFFF)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Invalid JTAG Lock Password Length!");
	}
	else
	{
		if (pwdLength != (pwd->size() - 1))
		{
			throw std::runtime_error("[ERROR_MESSAGE]Given JTAG Lock Password Length is not aligned with given password bytes!");
		}
		else
		{
			for (uint16_t i = 0; i < pwdLength; i++)
			{
				jtagLockPassword.data.push_back(convertHexStringToByte(pwd->at(i + 1)));
			}
		}
	}

}

DataBlock* Util::retPtrJtagLockSignature()
{
	return &jtagLockSignature;
}

DataBlock* Util::ptrDefaultPass()
{
	return &defaultPass;
}

void Util::initFamilyEnum()
{
#define MAKE_STRINGS_FAMILY(VAR) #VAR,
	const char* const  temp[] = {
		FAMILY(MAKE_STRINGS_FAMILY)
	};
	uint8_t size = sizeof(temp) / sizeof(temp[0]);
	familyVector.clear();
	for (uint8_t i = 0; i < size; i++)
	{
		std::string str = temp[i];
		if (str == "F5xx")
		{
			str = "5xx";
		}
		if( str == "F6xx")
		{
			str = "6xx";
		}
		if (str == "F543xfamily")
		{
			str = "543x_family";
		}
		familyVector.push_back(str);
	}
}

std::vector<std::string>* Util::ptrFamilyVector()
{
	return  &familyVector;
}

Family Util::generalizeFamily(Family fam)
{
	//Make the P4xx family general
	if ((fam == Family::P4xx) ||
		(fam == Family::MSP432P4xx))
	{
		fam = Family::P4xx;
	}
	//Make the F543x family general
	else if ((fam == Family::MSP430F543x) ||
		(fam == Family::F543x) ||
		(fam == Family::F543xfamily))
	{
		fam = Family::F543x;
	}
	//Make the Flash family general
	else if ((fam == Family::MSP430F5xx) ||
		(fam == Family::F5xx) ||
		(fam == Family::F6xx) ||
		(fam == Family::MSP430F6xx))
	{
		fam = Family::Flash;
	}
	//Make the Fram family general
	else if ((fam == Family::FRxx) ||
		     (fam == Family::MSP430FRxx) )
	{
		fam = Family::Fram;
	}
	else if ((fam == Family::E4xx) ||
		     (fam == Family::MSP432E4xx))
	{
		fam = Family::E4xx;
	}
	else
	{
		fam = Family::Crypto;
	}

	return fam;
}


bool Util::combinationFamilyProtocol(Family fam, Protocol protocol)
{
	// Available protocol for MSP432P4xx are: UART, I2C, SPI
	if (fam == Family::P4xx)
	{
		if ((protocol == Protocol::Uart) || (protocol == Protocol::I2c) || (protocol == Protocol::Spi))
		{
			return true;
		}
	}
	// Available protocol for MSP430F543x are: UART
	else if (fam == Family::F543x)
	{
		if (protocol == Protocol::Uart)
		{
			return true;
		}
	}
	// Available protocol for MSP430F5xx/6xx family are: UART and USB
	else if (fam == Family::Flash)
	{
		if ((protocol == Protocol::Uart) || (protocol == Protocol::Usb))
		{
			return true;
		}
	}
	// Available protocol for MSP430FRxx and CryptoBSL are: UART and I2C
	else if ((fam == Family::Fram) || (fam == Family::Crypto))
	{
		if ((protocol == Protocol::Uart) || (protocol == Protocol::I2c))
		{
			return true;
		}
	}
	//Available protocol for MSP432E4xx: UART, I2C, SPI, Ethernet, and USB DFU
	else if (fam == Family::E4xx)
	{
		if ((protocol == Protocol::Uart) || (protocol == Protocol::I2c) ||
			(protocol == Protocol::Spi) || (protocol == Protocol::Ethernet) ||
			(protocol == Protocol::Usb))
		{
			return true;
		}
	}
	return false;
}


void Util::initProtocolEnum()
{
#define MAKE_STRINGS_PROTOCOL(VAR) #VAR,
	const char* const  temp[] = {
		PROTOCOL(MAKE_STRINGS_PROTOCOL)
	};
	uint8_t size = sizeof(temp) / sizeof(temp[0]);
	protocolVector.clear();
	for (uint8_t i = 0; i < size; i++)
	{
		protocolVector.push_back(temp[i]);
	}
}

std::vector<std::string>* Util::ptrProtocolVector()
{
	return  &protocolVector;
}

void Util::initCommands()
{
#define MAKE_STRINGS_COMMANDS(VAR) #VAR,
	const char* const  temp[] = {
		COMMANDS(MAKE_STRINGS_COMMANDS)
	};
	uint8_t size = sizeof(temp) / sizeof(temp[0]);
	commandsVector.clear();
	for (uint8_t i = 0; i < size; i++)
	{
		commandsVector.push_back(temp[i]);
	}
}

std::vector<std::string>* Util::ptrCommandsVector()
{
	return  &commandsVector;
}


void Util::initUartBaudRateEnum()
{
	uartBaudRateVector.clear();
	uartBaudRateVector.push_back(9600);
	uartBaudRateVector.push_back(19200);
	uartBaudRateVector.push_back(38400);
	uartBaudRateVector.push_back(57600);
	uartBaudRateVector.push_back(115200);
}

std::vector<std::uint32_t >* Util::ptrUartBaudRateVector()
{
	return  &uartBaudRateVector;
}

void Util::initI2cBaudRateEnum()
{
	i2cBaudRateVector.clear();
	i2cBaudRateVector.push_back(100000);
	i2cBaudRateVector.push_back(400000);
}
std::vector<std::uint32_t>* Util::ptrI2cBaudRateVector()
{
	return  &i2cBaudRateVector;
}


void Util::initSpiBaudRateEnum()
{
	spiBaudRateVector.clear();
	spiBaudRateVector.push_back(125000);
	spiBaudRateVector.push_back(250000);
	spiBaudRateVector.push_back(500000);
	spiBaudRateVector.push_back(1000000);
}

std::vector<std::uint32_t>* Util::ptrSpiBaudRateVector()
{
	return  &spiBaudRateVector;
}

uint32_t Util::checkBaudRate(Protocol protocol, uint32_t baudInput, bool uartBaudFound, bool i2cBaudFound, bool spiBaudFound)
{
	if ((protocol == Protocol::Uart && uartBaudFound) ||
		(protocol == Protocol::I2c && i2cBaudFound) ||
		(protocol == Protocol::Spi && spiBaudFound))
	{
		return baudInput;
	}

	//Set baud rate to the default one
	else  if (protocol == Protocol::Uart && !uartBaudFound)
	{
		std::cout << "\tBaud rate setup for UART is not given / wrong" << std::endl;
		std::cout << "\tIt will be setup to the default 9600baud" << std::endl;
		return 9600;
	}
	else  if (protocol == Protocol::I2c && !i2cBaudFound)
	{
		std::cout << "\tBaud rate setup for I2C is not given / wrong" << std::endl;
		std::cout << "\tIt will be setup to the default 100kHz" << std::endl;
		return 100000;
	}
	else  if (protocol == Protocol::Spi && !spiBaudFound)
	{
		std::cout << "\tBaud rate setup for I2C is not given / wrong" << std::endl;
		std::cout << "\tIt will be setup to the default 125kHz" << std::endl;
		return 125000;
	}
	return 9600;
}

bool Util::isEthernetParamsValid(std::string macAddr, std::string clientIpAddr, std::string serverIpAddr)
{
	//Check MAC address
	boost::char_separator<char> sep(":-\r");
	boost::tokenizer<boost::char_separator<char>> tokens(macAddr, sep);
	macAddressVector.clear();

	//The conversion below returns error if the hexadecimal is invalid
	for (const std::string& t : tokens)
	{
		macAddressVector.push_back(this->convertHexStringToByte(t));
	}

	//MAC address shall have 6 bytes data 
	if (macAddressVector.size() != MAC_NUM_OF_BYTES)
	{
		throw std::runtime_error("\t[ERROR_MESSAGE]MAC address given is not valid!");
	}

	boost::system::error_code ec;
	//Check IP address
	try
	{
		boost::asio::ip::address ipv4 = boost::asio::ip::address::from_string(clientIpAddr, ec);
		if (serverIpAddr != "0.0.0.0")
		{
			ipv4 = boost::asio::ip::address::from_string(serverIpAddr, ec);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		throw std::runtime_error("\t[ERROR_MESSAGE]IP address given is not valid!");
	}

	return true;
}

std::vector<uint8_t> Util::retMacAddressVector()
{
	return macAddressVector;
}

bool Util::isRxCommand(Command cmd)
{
	if ((cmd == Command::RX_DATA_BLOCK) ||
		(cmd == Command::RX_DATA_BLOCK_32) ||
		(cmd == Command::RX_DATA_BLOCK_FAST) ||
		(cmd == Command::RX_PASSWORD) ||
		(cmd == Command::RX_PASSWORD_32) || 
		(cmd == Command::RX_SECURE_DATA_BLOCK) ||
		(cmd == Command::CUSTOM_COMMAND) ||
		(cmd == Command::JTAG_LOCK) ||
		(cmd == Command::JTAG_PASSWORD))
	{
		return true;
	}
	return false;
}

bool Util::isTxCommand(Command cmd)
{
	if ((cmd == Command::TX_DATA_BLOCK) ||
		(cmd == Command::TX_DATA_BLOCK_32))
	{
		return true;
	}
	return false;
}

bool Util::isPcCommand(Command cmd)
{
	if ((cmd == Command::DELAY) ||
		(cmd == Command::VERBOSE) ||
		(cmd == Command::APP_VERSION))
	{
		return true;
	}
	return false;
}

bool Util::isUnexecutedLine(std::string line)
{
	if (compareString(line.substr(0, 2), "//") ||
		(line == ""))
	{
		return true;
	}
	return false;
}
			 
bool Util::compareString(std::string inputString1, std::string inputString2)
{
	boost::to_lower(inputString1);
	boost::to_lower(inputString2);

	if (inputString1.compare(inputString2) != 0)
	{
		return false;
	}
	return true;
}

bool Util::compareInt(uint32_t inputInt1, uint32_t inputInt2)
{
	if (inputInt1 != inputInt2)
	{
		return false;
	}
	return true;
}

std::vector<std::string> Util::getTokens(std::string line)
{
	std::vector<std::string> temp;
	temp.clear();

	boost::char_separator<char> sep("\t\r ");
	boost::tokenizer<boost::char_separator<char>> tokens(line, sep);

	for (const std::string& t : tokens)
	{
		temp.push_back(t);
	}
	return temp;
}
	   
bool Util::checkExistence(std::string param, std::vector<std::string> *lookUpVector)
{
	for (uint8_t i = 0; i < (*lookUpVector).size(); i++)
	{
		if (compareString(param, (*lookUpVector).at(i)))
		{
			numToEnum = i;
			return true;
		}
	}
	return false;
}

bool Util::checkExistence(std::string param, std::vector<uint32_t> *lookUpVector)
{
	const char* p = param.c_str();
	uint32_t paramInt = std::strtoul(p, NULL, 10);
	for (uint8_t i = 0; i < (*lookUpVector).size(); i++)
	{
		if (compareInt(paramInt, (*lookUpVector).at(i)))
		{
			numToEnum = i;
			return true;
		}
	}
	return false;
}

bool Util::checkExistence(std::string param1, std::string master, uint8_t numOfCharsCompared)
{
	std::string param1Cut = param1.substr(0, numOfCharsCompared);

	if (compareString(param1Cut, master))
	{
		return true;
	}
	return false;
}

uint8_t Util::retNumToEnum()
{
	return	numToEnum;
}

uint8_t Util::convertHexStringToByte(std::string hex)
{
	hex.erase(std::remove(hex.begin(), hex.end(), '\r'), hex.end());
	hex.erase(std::remove(hex.begin(), hex.end(), '\t'), hex.end());
	uint8_t tmp = 0;
	if (compareString((hex.substr(0, 2)), "0x"))
	{
		hex = hex.substr(2, hex.size());
	}
	if (hex.size() != 2)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Wrong hexadecimal input to be converted to byte :" + hex);
	}
	else
	{
		std::size_t pos = 0;
		tmp = (uint8_t)std::stoul(hex, &pos, 16);
		if (pos == hex.size())
		{
			return tmp;
		}
		else
		{
			throw std::runtime_error("[ERROR_MESSAGE]Wrong hexadecimal input:" + hex);
		}
	}
}

uint32_t Util::convertHexStringToUlong(std::string hex)
{
	hex.erase(std::remove(hex.begin(), hex.end(), '\r'), hex.end());
	hex.erase(std::remove(hex.begin(), hex.end(), '\t'), hex.end());
	uint32_t tmp = 0;
	if (compareString((hex.substr(0, 2)), "0x"))
	{
		hex = hex.substr(2, hex.size());
	}
	std::size_t pos = 0;
	tmp = std::stoul(hex, &pos, 16);
	if (pos == hex.size())
	{
		return tmp;
	}
	else
	{
		throw std::runtime_error("[ERROR_MESSAGE]Wrong hexadecimal input:" + hex);
	}
}

uint32_t Util::convertNumStringToUlong(std::string num)
{
	num.erase(std::remove(num.begin(), num.end(), '\r'), num.end());
	num.erase(std::remove(num.begin(), num.end(), '\t'), num.end());
	uint32_t tmp = 0;
	std::size_t pos = 0;
	tmp = std::stoul(num, &pos, 10);
	if (pos == num.size())
	{
		return tmp;
	}
	else
	{
		throw std::runtime_error("[ERROR_MESSAGE]Wrong decimal parameters : " + num);
	}
}

std::string Util::convertByteToString(uint8_t input, bool usePrefix)
{
	std::stringstream buf;
	buf << std::setfill('0')
		<< std::setw(sizeof(input) * 2)
		<< std::hex
		<< (uint32_t)input;

	if (usePrefix)
		return ("0x" + buf.str());
	else
		return buf.str();

}

std::string Util::convertUlongToString(uint32_t input, bool usePrefix)
{
	std::stringstream buf;
	buf << std::setfill('0')
		<< std::hex
		<< (uint32_t)input;

	if (usePrefix)
		return ("0x" + buf.str());
	else
		return buf.str();
}

uint16_t Util::concatenateBytesToShort(std::vector<uint8_t> *input, uint32_t startIdx)
{
	return (((uint16_t)input->at(startIdx + 1)) << 8) |
		   input->at(startIdx);
}

uint32_t Util::concatenateBytesToUlong(std::vector<uint8_t> *input, uint32_t startIdx)
{
	return (((uint32_t)input->at(startIdx+3)) << 24) |
		   (((uint32_t)input->at(startIdx+2)) << 16) |
		   (((uint32_t)input->at(startIdx+1)) << 8)  |
		   input->at(startIdx);
}


std::string Util::fillInZero(std::string input, uint8_t numOfBytes)
{
	std::string zeroFill;
	for (uint8_t i = 0; i < (numOfBytes - input.size()); i++)
	{
		zeroFill += "0";
	}
	std::string tmp = zeroFill + input;
	boost::to_upper(tmp);
	return tmp;
}

void Util::getTokens(std::string input, std::vector<std::string>* vct)
{
	std::vector<std::string> tmp;

	if (!compareString(input.substr(0, 1), "["))
	{
		throw std::runtime_error("Wrong brackets for union of parameters!");
	}
	else
	{
		std::string noBracket = input.substr(1, input.size() - 2);
		boost::char_separator<char> sep(","); 
		boost::tokenizer<boost::char_separator<char>> tokens(noBracket, sep);

		for (const std::string& t : tokens)
		{
			tmp.push_back(t);
		}

		for (uint8_t i = 0; i < tmp.size(); i++)
		{
			
			bool findDash = (tmp.at(i).find("-") != std::string::npos); 
			bool findTxt = (tmp.at(i).find(".txt") != std::string::npos); 
			bool findHex = (tmp.at(i).find(".hex") != std::string::npos); 

			// if there is no ".txt" or ".hex" in the tokens, the "-" would be the
			// tokens separator
			if (findDash && !(findTxt || findHex))
			{
				boost::char_separator<char> sepDash("-");
				boost::tokenizer<boost::char_separator<char>> tokensDash(tmp.at(i), sepDash);
				for (const std::string& t : tokensDash)
				{
					vct->push_back(t);
				}
			}
			// the '-' inside file name should not be the tokens separator 
			else
			{
				vct->push_back(tmp.at(i));
			}

		}
	}
}

void Util::resetCrc()
{
	crc = 0xFFFF;
}
	 
uint8_t Util::getCheckSumLow()
{
	return (uint8_t)(crc & 0x00FF);
}

uint8_t Util::getCheckSumHigh()
{
	return (uint8_t)((crc >> 8) & 0x00FF);
}

uint16_t Util::retCrc()
{
	return crc;
}

void Util::resetCrcE4xx()
{
	crc = 0x0000;
}

uint8_t Util::retCrcE4xx()
{
	return (uint8_t)(0x00FF & crc);
}

void Util::checkSumCalculate(uint8_t data)
{
	uint8_t x;
	x = ((crc >> 8) ^ data) & 0xff;
	x ^= x >> 4;
	crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
}

void Util::checkSumCalculateE4xx(uint8_t data)
{
	crc += data;
}