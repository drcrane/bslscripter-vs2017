/*
==Util.h==

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

class Util
{
public:

	/*******************************************************************************
	*Constructor
	*******************************************************************************/
	Util();

	/*******************************************************************************
	*Constructor
	*******************************************************************************/
	~Util();

	/*******************************************************************************
	*Function:    compareString
	*Description: compare if two strings are equal.
	*             before comparison, both strings are converted to lowercase format.
	*Parameters:  std::string str1 : first string input
	*             std::string str2 : second string input
	*Returns:     boolean true: if two strings are equal
	*             boolean false: if two strings are not equal
	*******************************************************************************/
	bool compareString(std::string inputString1, std::string inputString2);

	/*******************************************************************************
	*Function:    compareInt
	*Description: compare if two integers are equal
	*Parameters:  uint32_t int1 : first integer input
	*             uint32_t int2 : second integer input
	*Returns:     boolean true: if two integers are equal
	*             boolean false: if two integers are not equal
	*******************************************************************************/
	bool compareInt(uint32_t inputInt1, uint32_t inputInt2);

	/*******************************************************************************
	*Function:    getTokens
	*Description: get the tokens of strings that are separated with space/tab
	              in script mode. the tokens later stored in std::vector of string
	*Parameters:  std::string line : input line
	*Returns:     std::vector<std::string>
	*******************************************************************************/
	std::vector<std::string> getTokens(std::string line);

	/*******************************************************************************
	*Function:    convertHexStringToByte
	*Description: convert hexadecimal in string into unsigned byte
	*Parameters:  std::string hex : input hexadecimal in string format
	*Returns:     uint8_t
	*******************************************************************************/
	uint8_t convertHexStringToByte(std::string hex);

	/*******************************************************************************
	*Function:    convertHexStringToUlong
	*Description: convert hexadecimal in string into unsigned long
	*Parameters:  std::string hex : input hexadecimal in string format
	*Returns:     uint32_t
	*******************************************************************************/
	uint32_t convertHexStringToUlong(std::string hex);

	/*******************************************************************************
	*Function:    convertNumStringToUlong
	*Description: convert decimal represented in string into unsigned long
	*Parameters:  std::string num : input decimal in string format
	*Returns:     uint32_t
	*******************************************************************************/
	uint32_t convertNumStringToUlong(std::string num);

	/*******************************************************************************
	*Function:    convertByteToString
	*Description: convert unsigned byte into hexadecimal in string format
	*Parameters:  uint8_t input : input decimal in string format
	              bool usePrefix: using '0x' prefix if it is true
	*Returns:     std::string
	*******************************************************************************/
	std::string convertByteToString(uint8_t input, bool usePrefix);

	/*******************************************************************************
	*Function:    convertUlongToString
	*Description: convert unsigned long into hexadecimal in string format
	*Parameters:  uint32_t input: input decimal in string format
	              bool usePrefix: using '0x' prefix if it is true
	*Returns:     std::string
	*******************************************************************************/
	std::string convertUlongToString(uint32_t input, bool usePrefix);

	/*******************************************************************************
	*Function:    concatenateBytesToShort
	*Description: concatenate 2 dataof uint8_t into uint16_t data
	*Parameters:  std::vector<uint8_t> *input : ptr to input vector
	              uint32_t startIdx : first idx in the vector which is the start
	                                  of byte to be concatenate
	*Returns:     std::uint16_t
	*******************************************************************************/
	uint16_t concatenateBytesToShort(std::vector<uint8_t> *input, uint32_t startIdx);

	/*******************************************************************************
	*Function:    concatenateBytesToUlong
	*Description: concatenate 4 dataof uint8_t into uint32_t data
	*Parameters:  std::vector<uint8_t> *input : ptr to input vector
	              uint32_t startIdx : first idx in the vector which is the start
				                      of byte to be concatenate
	*Returns:     std::uint32_t
	*******************************************************************************/
	uint32_t concatenateBytesToUlong(std::vector<uint8_t> *input, uint32_t startIdx);

	/*******************************************************************************
	*Function:    fillInZero
	*Description: fill in zero in prexis for hexadecimal in string format
	*Parameters:  std::string input: input hexadecimal in string format
	              uint8_t numOfByte: number of characters intended
	*Returns:     std::string
	*******************************************************************************/
	std::string fillInZero(std::string input, uint8_t numOfBytes);

	/*******************************************************************************
	*Function:    getTokens
	*Description: get the tokens of strings for union parameters in CLI mode
	that is written with the square brackets[]
	*Parameters:  std::string line : input line
	std::vector<std::string>* vct : vector for the tokens result
	*Returns:     -
	*******************************************************************************/
	void getTokens(std::string input, std::vector<std::string>* vct);

	/*******************************************************************************
	*Function:    ptrCommandsVector
	*Description: return pointer of commandsVector
	*Parameters:  -
	*Returns:     std::vector<std::string>*
	*******************************************************************************/
	std::vector<std::string>* ptrCommandsVector();

	/*******************************************************************************
	*Function:    ptrFamilyVector
	*Description: return pointer of familyVector
	*Parameters:  -
	*Returns:     std::vector<std::string>*
	*******************************************************************************/
	std::vector<std::string>* ptrFamilyVector();

	/*******************************************************************************
	*Function:    ptrProtocolVector
	*Description: return pointer of protocolVector
	*Parameters:  -
	*Returns:     std::vector<std::string>*
	*******************************************************************************/
	std::vector<std::string>* ptrProtocolVector();

	/*******************************************************************************
	*Function:    ptrUartBaudRateVector
	*Description: return pointer of uartBaudRateVector
	*Parameters:  -
	*Returns:     std::vector<std::string>*
	*******************************************************************************/
	std::vector<uint32_t>* ptrUartBaudRateVector();

	/*******************************************************************************
	*Function:    ptrI2cBaudRateVector
	*Description: return pointer of i2cBaudRateVector
	*Parameters:  -
	*Returns:     std::vector<std::string>*
	*******************************************************************************/
	std::vector<uint32_t>* ptrI2cBaudRateVector();

	/*******************************************************************************
	*Function:    ptrSpiBaudRateVector
	*Description: return pointer of spiBaudRateVector
	*Parameters:  -
	*Returns:     std::vector<std::string>*
	*******************************************************************************/
	std::vector<uint32_t>* ptrSpiBaudRateVector();

	/*******************************************************************************
	*Function:    initDefaultPassword
	*Description: initialize the default password
	*Parameters:  uint16_t size: either 16, 32, or 256 depends on the family
	*Returns:     -
	*******************************************************************************/
	void initDefaultPassword(uint16_t size);

	/*******************************************************************************
	*Function:    ptrDefaultPass
	*Description: return the pointer fo defaultPass buffer
	*Parameters:  -
	*Returns:     DataBlock*
	*******************************************************************************/
	DataBlock* ptrDefaultPass();

	/*******************************************************************************
	*Function:    checkExistence
	*Description: check if the param as input is availabe in the lookUpVector
	*Parameters:  std::string param: input to be checked
	std::vector<std::string> *lookUpVector: look up table
	*Returns:     bool
	*******************************************************************************/
	bool checkExistence(std::string param, std::vector<std::string> *lookUpVector);

	/*******************************************************************************
	*Function:    checkExistence
	*Description: check if the param as input is availabe in the lookUpVector
	*Parameters:  std::string param: input to be checked
	              std::vector<std::uint32_t> *lookUpVector: look up table
	*Returns:     bool
	*******************************************************************************/
	bool checkExistence(std::string param, std::vector<uint32_t> *lookUpVector);

	/*******************************************************************************
	*Function:    checkExistence
	*Description: check if the substring param as input is match with the master
	*Parameters:  std::string param1: input to be checked
	              std::string master: string reference
	              uint8_t numOfCharsCompared: number of chard of param1 to be
	              compared
	*Returns:     bool
	*******************************************************************************/
	bool checkExistence(std::string param1, std::string master, uint8_t numOfCharsCompared);

	/*******************************************************************************
	*Function:    retNumToEnum
	*Description: return the enum index from look up vector
	*Parameters:  -
	*Returns:     uint8_t
	*******************************************************************************/
	uint8_t retNumToEnum();

	/*******************************************************************************
	*Function:    generalizeFamily
	*Description: generalize family into:
	              Flash, Fram, Crypto, F543x_family, or P4xx
	*Parameters:  Family fam: input parameter
	*Returns:     Family
	*******************************************************************************/
	Family generalizeFamily(Family fam);

	/*******************************************************************************
	*Function:    combinationFamilyProtocol
	*Description: check if the combination between family and protocol is valid
	              for initialization
	*Parameters:  Family fam: family input parameter
	              Protocol protocol: protocol input parameter
	*Returns:     bool
	*******************************************************************************/
	bool combinationFamilyProtocol(Family fam, Protocol protocol);

	/*******************************************************************************
	*Function:    checkBaudRate
	*Description: check if the combination between protocol  and the given baudrate
	if baudrate input is not given, it will be set into the default
	baudrwate
	*Parameters:  Protocol protocol: protocol input parameter
	              uint32_t baudInput: given baudRate
	              bool uartBaudFound: uart baud rate is given
	              bool i2cBaudFound:  i2c baud rate is given
	              bool spiBaudFound:  spi baud rate is given
	*Returns:     uint32_t
	*******************************************************************************/
	uint32_t checkBaudRate(Protocol protocol, uint32_t baudInput, bool uartBaudFound, bool i2cBaudFound, bool spiBaudFound);

	/*******************************************************************************
	*Function:    isEthernetParamsValid
	*Description: check if the MAC address and Client IP for Ethernet bootloader
	              is valid
	*Parameters:  std::string macAddr: given MAC address
	              std::string ipAddr: given Client IP address
	*Returns:     bool
	*******************************************************************************/
	bool isEthernetParamsValid(std::string macAddr, std::string clientIpAddr, std::string serverIpAddr);

	/*******************************************************************************
	*Function:    retMacAddressVector
	*Description: return the vector of MAC address
	*Parameters:  -
	*Returns:     macAddressVector
	*******************************************************************************/
	std::vector<uint8_t> retMacAddressVector();

	/*******************************************************************************
	*Function:    isRxCommand
	*Description: check if the command is either:
	              RX_DATA_BLOCK, RX_DATA_BLOCK_32, RX_DATA_BLOCK_FAST,
				  CUSTOM_COMMAND, JTAG_LOCK, JTAG_PASSWORD,
				  RX_SECURE_DATA_BLOCK,RX_PASSWORD,RX_PASSWORD_32
	*Parameters:  Command cmd: command parameter
	*Returns:     bool
	*******************************************************************************/
	bool isRxCommand(Command cmd);

	/*******************************************************************************
	*Function:    isTxCommand
	*Description: check if the command is either:
	              TX_DATA_BLOCK, TX_DATA_BLOCK_32
	*Parameters:  Command cmd: command parameter
	*Returns:     bool
	*******************************************************************************/
	bool isTxCommand(Command cmd);

	/*******************************************************************************
	*Function:    isPcCommand
	*Description: check if the command is either:
	              DELAY, VERBOSE	
	*Parameters:  Command cmd: command parameter
	*Returns:     bool
	*******************************************************************************/
	bool isPcCommand(Command cmd);

	/*******************************************************************************
	*Function:    isUnexecutedLine
	*Description: check if the line is a comment line or not
	*Parameters:  std::string line: input line
	*Returns:     bool
	*******************************************************************************/
	bool isUnexecutedLine(std::string line);

	/*******************************************************************************
	*Function:    resetCrc
	*Description: reset crc variable to start new crc calculation
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void resetCrc();

	void resetCrcE4xx();

	/*******************************************************************************
	*Function:    retCrc
	*Description: return crc calculation result
	*Parameters:  -
	*Returns:     uint16_t
	*******************************************************************************/
	uint16_t retCrc();

	uint8_t retCrcE4xx();

	/*******************************************************************************
	*Function:    getCheckSumLow
	*Description: get crc LSB to be Check Sum bytes in UART protocol
	*Parameters:  -
	*Returns:     uint8_t
	*******************************************************************************/
	uint8_t getCheckSumLow();

	/*******************************************************************************
	*Function:    getCheckSumHigh
	*Description: get crc MSB to be Check Sum bytes in UART protocol
	*Parameters:  -
	*Returns:     uint8_t
	*******************************************************************************/
	uint8_t getCheckSumHigh();

	/*******************************************************************************
	*Function:    checkSumCalculate
	*Description: calculate the crc in crc variable
	*Parameters:  uint8_t data: input byte
	*Returns:     -
	*******************************************************************************/
	void checkSumCalculate(uint8_t data);

	void checkSumCalculateE4xx(uint8_t data);


	/*******************************************************************************
	*Function:    initJtagLockSignature
	*Description: init the buffer for jtag lock signature
	*Parameters:  ModeParams modeParams: mode param to get family identity
	*Returns:     -
	*******************************************************************************/
	void initJtagLockSignature(ModeParams modeParams);

	/*******************************************************************************
	*Function:    retPtrJtagLockSignature
	*Description: return the pointer of jtagLockSignature buffer
	*Parameters:  -
	*Returns:     DataBlock*
	*******************************************************************************/
	DataBlock* retPtrJtagLockSignature();

	/*******************************************************************************
	*Function:    retPtrJtagLockSignature
	*Description: return the pointer of jtagLockPassword buffer
	*Parameters:  -
	*Returns:     DataBlock*
	*******************************************************************************/
	DataBlock* retPtrJtagLockPassword();

	/*******************************************************************************
	*Function:    setJtagPasswordForFram
	*Description: set the JTAG password for JTAG lock in FRAM
	*Parameters:  std::vector<std::string> *pwd: JTAG password input
	*Returns:     -
	*******************************************************************************/
	void setJtagPasswordForFram(std::vector<std::string> *pwd);

protected:

	/***********************************************************************************
	* Class veriabels
	***********************************************************************************/
	std::vector<std::string> commandsVector;
	std::vector<std::string> familyVector;
	std::vector<std::string> protocolVector;
	std::vector<uint32_t> uartBaudRateVector;
	std::vector<uint32_t> i2cBaudRateVector;
	std::vector<uint32_t> spiBaudRateVector;
	DataBlock defaultPass;
	DataBlock jtagLockSignature;
	DataBlock jtagLockPassword;

private:

	static const uint8_t MAC_NUM_OF_BYTES = 6;
 
	uint8_t numToEnum;
	uint16_t crc;
	std::vector<uint8_t> macAddressVector;

	/*******************************************************************************
	*Function:    initFamilyEnum
	*Description: initialize look up vector for Family definition
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void initFamilyEnum();

	/*******************************************************************************
	*Function:    initProtocolEnum
	*Description: initialize look up vector for Protocol definition
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void initProtocolEnum();

	/*******************************************************************************
	*Function:    initCommands
	*Description: initialize look up vector for Commands definition
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void initCommands();

	/*******************************************************************************
	*Function:    initUartBaudRateEnum
	*Description: initialize look up vector for UART baud rate definition
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void initUartBaudRateEnum();

	/*******************************************************************************
	*Function:    initI2cBaudRateEnum
	*Description: initialize look up vector for I2C baud rate definition
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void initI2cBaudRateEnum();

	/*******************************************************************************
	*Function:    initSpiBaudRateEnum
	*Description: initialize look up vector for SPI baud rate definition
	*Parameters:  -
	*Returns:     -
	*******************************************************************************/
	void initSpiBaudRateEnum();
};

