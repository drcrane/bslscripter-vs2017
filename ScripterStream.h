/*
==ScripterStream.h==

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

class ScripterStream
{
public:

	static const uint8_t RECT_BRACKET = 0x01;
	static const uint8_t TRI_BRACKET = 0x02;
	static const uint8_t READ_MODE = 0x00;
	static const uint8_t WRITE_MODE = 0x01;


	/***********************************************************************************
	* Constructor
	***********************************************************************************/
	ScripterStream(Util *util, bool cliMode);

	/***********************************************************************************
	* Deconstructor
	***********************************************************************************/
	~ScripterStream();

	/***********************************************************************************
	*Function:	  setBufferProperties
	*Description: define the size of the buffer to hold data for the transmission,
	              number of header and checksum, and segment size. Those parameters
				  depend on the protocol
	*Parameters:  uint16_t maxBuffer: maximum number of bytes data in one packet
	              uint16_t numHeader: number bytes of header byte, NH-NL bytes, and
				    command byte
				  uint16_t checkSum: number bytes of checksum
				  uint16_t segmentSize: size of memory segment based on the family
	*Returns:	  -
	***********************************************************************************/
	void setBufferProperties(uint16_t maxBuffer, uint16_t numHeader, uint16_t checkSum, uint16_t segmentSize);

 	/***********************************************************************************
	*Function:	  retNumOfHeader
	*Description: return number bytes of header byte, NH-NL bytes, and command byte
	*Parameters:  -
	*Returns:	  uint32_t
	***********************************************************************************/
	uint32_t retNumOfHeader();

 	/***********************************************************************************
	*Function:	  retNumOfDataPerPacket
	*Description: return number bytes of data per packet
	*Parameters:  -
	*Returns:	  uint32_t
	***********************************************************************************/
	uint32_t retNumOfDataPerPacket();

  	/***********************************************************************************
	*Function:	  retNumberOfCheckSum
	*Description: return number bytes checksum
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	uint32_t retNumberOfCheckSum();

	/***********************************************************************************
	*Function:	  startScripter
	*Description: open the streaming script file and get the path directory
	*Parameters:  char * ch: file name of script file
	*Returns:	  std::string 
	***********************************************************************************/
	std::string startScripter(char * ch);

	/***********************************************************************************
	*Function:	  closeScriptFile
	*Description: close the streaming script file 
	*Parameters:  -
	*Returns:	  .
	***********************************************************************************/
	void closeScriptFile();

	/***********************************************************************************
	*Function:	  getLine
	*Description: check if command still available or end of file is reached
	*Parameters:  -
	*Returns:	  bool
	***********************************************************************************/
	bool getLine();

	/***********************************************************************************
	*Function:	  retStringCommand
	*Description: return string line from the script file
	*Parameters:  -
	*Returns:	  std::string
	***********************************************************************************/
	std::string retStringCommand();

	/***********************************************************************************
	*Function:	  initReadFile
	*Description: initiate the pointer to streaming input file, check if the file exists
	*Parameters:  std::string filePath: input file name
	*             std::string mainPathDir: path relative from the script file
	*Returns:     -
	***********************************************************************************/
	void initReadFile(std::string filePath, std::string mainPathDir);

	/***********************************************************************************
	*Function:	  isTxtFile
	*Description: check if the input file has .txt format
	*Parameters:  -
	*Returns:     bool
	***********************************************************************************/
	bool isTxtFile();

	/***********************************************************************************
	*Function:	  isHexFile
	*Description: check if the input file has .hex format
	*Parameters:  -
	*Returns:     bool
	***********************************************************************************/
	bool isHexFile();

	/***********************************************************************************
	*Function:	  readTxtFile
	*Description: read the .txt file and get the data bytes and store it in dataBlock
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readTxtFile();

	/***********************************************************************************
	*Function:	  readTxtFileE4xx
	*Description: read the .txt file and get the data bytes and store it in dataBlock
	              the reading mechanism in this function belong to the MSP432E4xx
				  with serial communication protocol (UART, I2C, SPI)
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readTxtFileE4xx();

	/***********************************************************************************
	*Function:	  readTxtFileE4xxEthernet
	*Description: read the .txt file and get the data bytes and store it in dataBlock
	              the reading mechanism in this function belong to the MSP432E4xx
				  with Ethernet protocol
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readTxtFileE4xxEthernet();

	/***********************************************************************************
	*Function:	  readTxtFileE4xxUsbDfu
	*Description: read the .txt file and get the data bytes and store it in dataBlock
	              the reading mechanism in this function belong to the MSP432E4xx
				  with USB DFU protocol
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readTxtFileE4xxUsbDfu();

	/***********************************************************************************
	*Function:	  readHexFile
	*Description: read the .hex file and get the data bytes	and store it in dataBlock
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readHexFile();

	/***********************************************************************************
	*Function:	  readHexFile
	*Description: read the .hex file and get the data bytes and store it in dataBlock
	              the reading mechanism in this function belong to the MSP432E4xx
				  with serial protocol
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readHexFileE4xx();

	/***********************************************************************************
	*Function:	  readHexFile
	*Description: read the .hex file and get the data bytes and store it in dataBlock
	              the reading mechanism in this function belong to the MSP432E4xx
				  with Ethernet protocol
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readHexFileE4xxEthernet();

	/***********************************************************************************
	*Function:	  readHexFile
	*Description: read the .hex file and get the data bytes and store it in dataBlock
	              the reading mechanism in this function belong to the MSP432E4xx
	              with USB DFU protocol
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readHexFileE4xxUsbDfu();

	/***********************************************************************************
	*Function:	  clearDataBlock
	*Description: clear dataBlock
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void clearDataBlock();

	/***********************************************************************************
	*Function:	  retDataBlockRead
	*Description: return pointer dataBlock to be used in transmission
	*Parameters:  -
	*Returns:     DataBlock*
	***********************************************************************************/
	DataBlock* retDataBlockRead();

	/***********************************************************************************
	*Function:	  closeReadFile
	*Description: close streaming on read file
	*Parameters:  -
	*Returns:     DataBlock*
	***********************************************************************************/
	void closeReadFile();

	/***********************************************************************************
	*Function:	  retScripterReadStatus
	*Description: return scripter read file status
	*Parameters:  -
	*Returns:     ScripterReadStatus
	***********************************************************************************/
	ScripterReadStatus retScripterReadStatus();

	/***********************************************************************************
	*Function:	  retReadLineStatus
	*Description: return scripter read line status
	*Parameters:  -
	*Returns:     ReadLineStatus
	***********************************************************************************/
	ReadLineStatus retReadLineStatus();

	/***********************************************************************************
	*Function:	  
	*Description: 
	*Parameters:  
	*Returns:     
	***********************************************************************************/
	std::string retPcAddressIntelHex();

	/***********************************************************************************
	*Function:
	*Description:
	*Parameters:
	*Returns:
	***********************************************************************************/
	void resetIntelHex03detect();

	/***********************************************************************************
	*Function:
	*Description:
	*Parameters:
	*Returns:
	***********************************************************************************/
	bool retIntelHex03detect();

	/***********************************************************************************
	*Function:	  initWriteFile
	*Description: initiliaze the output file to write the data from transmission
	*Parameters:  std::string filePath: file name of the output file
	              std::string address: start address where the memory located
				  std::string mainPathDir: relative path of the script file location
	*Returns:     ReadLineStatus
	***********************************************************************************/
	void initWriteFile(std::string filePath, std::string address, std::string mainPathDir);

	/***********************************************************************************
	*Function:	  writeTxtFile
	*Description: write the data from transmission to the .txt file
	*Parameters:  std::vector<uint8_t> *buf: data bytes to be written on the file
	              uint32_t sizeBuf: number of bytes contain in *buf
	*Returns:     -
	***********************************************************************************/
	void writeTxtFile(std::vector<uint8_t> *buf, uint32_t sizeBuf);

	/***********************************************************************************
	*Function:	  writeHexFile
	*Description: write the data from transmission to the .hex file
	*Parameters:  std::vector<uint8_t> *buf: data bytes to be written on the file
	uint32_t sizeBuf: number of bytes contain in *buf
	*Returns:     -
	***********************************************************************************/
	void writeHexFile(std::vector<uint8_t> *buf, uint32_t sizeBuf);

	/***********************************************************************************
	*Function:	  retBytesWritten
	*Description: return number daty bytes that been written to the output file
	*Parameters:  -
	*Returns:     uint32_t
	***********************************************************************************/
	uint32_t retBytesWritten();

	/***********************************************************************************
	*Function:	  closeWriteFile
	*Description: close the streaming on the output file
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void closeWriteFile();

	/***********************************************************************************
	*Function:	  initReadCustomCommand
	*Description: initiate the pointer to streaming input file, check if the file exists
	*Parameters:  std::string filePath: input file name
	*             std::string mainPathDir: path relative from the script file
	*Returns:     -
	***********************************************************************************/
	void initReadCustomCommand(std::string filePath, std::string mainPathDir);

	/***********************************************************************************
	*Function:	  readCustomCommand
	*Description: read the .txt file and get the data bytes and store it in a buffer
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void readCustomCommand();

	/***********************************************************************************
	*Function:	  retPtrCustomCmdBuffer
	*Description: return pointer buffer of custom command to be used in transmission
	*Parameters:  -
	*Returns:     std::vector<uint8_t>*
	***********************************************************************************/
	std::vector<uint8_t> *retPtrCustomCmdBuffer();

	/***********************************************************************************
	*Function:	  initLogTxtFile
	*Description: initiate the pointer to streaming create log file
	*Parameters:  bool enLogInterpreter: will not generate log file if bool false
	*             std::string path : location of script file located
	*Returns:     -
	***********************************************************************************/
	void initLogTxtFile(bool enLogInterpreter, std::string path);

	/***********************************************************************************
	*Function:	  writeLogFile
	*Description: write the line of string to the log file
	*Parameters:  std::string line: line to be written on the log file
	              bool tab: insert the tab on the beginning of line
	*Returns:     -
	***********************************************************************************/
	void writeLogFile(std::string line, bool tab);

	/***********************************************************************************
	*Function:	  closeLogFile
	*Description: close the streaming on the log file
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void closeLogFile();

	/***********************************************************************************
	*Function:	  showVerbose
	*Description: show the bytes in the transmission line
	*Parameters:  std::vector<uint8_t> *txBuffer: buffer contains transmitted-bytes
	              std::vector<uint8_t> *rxBuffer: buffer contains received-bytes
				  uint32_t startIdx: idx from rxBuffer where the bytes will be shown
				  uint32_t numOfResponse: number of response bytes 
				  bool enVerbose: enable status of the verbose mode
	*Returns:     -
	***********************************************************************************/
	void showVerbose(std::vector<uint8_t> *txBuffer, std::vector<uint8_t> *rxBuffer, uint32_t startIdx, uint32_t numOfResponse, bool enVerbose);

	/***********************************************************************************
	*Function:	  showVerboseE4xx
	*Description: show the bytes in the transmission line specific for serial and 
	              ethernet procol mode
	*Parameters:  std::vector<uint8_t> *txBuffer: buffer contains transmitted-bytes
	              std::vector<uint8_t> *rxBuffer: buffer contains received-bytes
	              bool enVerbose: enable status of the verbose mode
	*Returns:     -
	***********************************************************************************/
	void showVerboseE4xx(std::vector<uint8_t> *txBuffer, std::vector<uint8_t> *rxBuffer, bool enVerbose);

	/***********************************************************************************
	*Function:	  showVerboseE4xxUsbDfu 
	*Description: show the bytes in the transmission line specific for serial and
	ethernet procol mode
	*Parameters:  std::vector<uint8_t> *txBuffer: buffer contains transmitted-bytes
	              std::vector<uint8_t> *rxBuffer: buffer contains received-bytes
				  uint16_t numOfTxBufferData : number of bytes shall be verbosed
				  uint16_t numOfRxBufferData : number of bytes shall be verbosed
	              bool enVerbose: enable status of the verbose mode
	*Returns:     -
	***********************************************************************************/
	void showVerboseE4xxUsbDfu(std::vector<uint8_t> *txBuffer, std::vector<uint8_t> *rxBuffer, uint16_t numOfTxBufferData, uint16_t numOfRxBufferData, bool enVerbose);

	/***********************************************************************************
	*Function:	  calcTransSpeed
	*Description: calculate the speed of the transmission
	*Parameters:  double time : total time elapsed
				  uint32_t numOfbytes : number of pure data bytes on the transmission
				  uint8_t mode : choose if it's reading/writing mode
	*Returns:     -
	***********************************************************************************/
	void calcTransSpeed(double time, uint32_t numOfbytes, uint8_t mode);

	/***********************************************************************************
	*Function:	  getThePathFile
	*Description: set the path for the respective file
	*Parameters:  -
	*Returns:     std::string 
	***********************************************************************************/
	std::string getThePathFile(std::string stInput, std::string relativePath);

	/***********************************************************************************
	*Function:	  getDataOfProgrammedMemory
	*Description: get the information about part of memory and its size that is 
	              programmed based on the input file
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void getDataOfProgrammedMemory();

	/***********************************************************************************
	*Function:	  retProgrammedMemory
	*Description: return the information of getDataOfProgrammedMemory() function that is
	              stored in  ProgrammedMemory data struct
	*Parameters:  -
	*Returns:     ProgrammedMemory*
	***********************************************************************************/
	ProgrammedMemory* retProgrammedMemory();

	/***********************************************************************************
	*Function:	  retCliMode
	*Description: return ifcommand line mode is chosen
	*Parameters:  -
	*Returns:     bool
	***********************************************************************************/
	bool retCliMode();

	/***********************************************************************************
	*Function:	  setEnQuiet
	*Description: set the quiet mode in CLI mode
	*Parameters:  bool enQuiet: enable status for the quiet mode in CLI mode
	*Returns:     -
	***********************************************************************************/
	void setEnQuiet(bool enQuiet);

	/***********************************************************************************
	*Function:	  getEnQuiet
	*Description: get the quiet mode status in CLI mode
	*Parameters:  -
	*Returns:     bool
	***********************************************************************************/
	bool getEnQuiet();

	/***********************************************************************************
	*Function:	  showHeaderFileStatus
	*Description: set the write log writings with no tab in the beginning
	*Parameters:  -
	*Returns:     -
	***********************************************************************************/
	void showHeaderFileStatus();

protected:

	/***********************************************************************************
	* Class variables
	***********************************************************************************/
	ReadLineStatus readLineStatus;
	ScripterReadStatus scripterReadStatus;
	bool hexFile;
	bool txtFile;

private:

	/***********************************************************************************
	* Class variables
	***********************************************************************************/
	Util *util;
	uint16_t maxBufferSize;
	uint16_t numOfHeader;
	uint16_t numOfCheckSum;
	uint32_t segmentSize;

	bool cliMode;
	std::string cliLineOfCommands;

	std::string fileScriptName;
	std::ifstream fileScriptStream;
	std::string line;
	boost::posix_time::ptime date_time;
	std::string showHeaderFile;

	//File pointer for reading and writing
	std::string fileName;

	//Reading task
	std::ifstream fileReadStream;
	uint16_t bytesRead;
	uint32_t saveLastReadAddress;
	DataBlock dataBlockRead;

	void getHexLineParams(std::string line, uint32_t actualSize);
	uint16_t hexLineByteCnt;
	uint32_t hexLineAddress;
	uint32_t extLineAddress;
	std::string pcAddressIntelHex;
	std::string storeLine;
	std::vector<uint8_t> leftOverE4xxHexFile;
	uint32_t cs;
	bool renewAddress;
	bool intelHex03detect;

	ProgrammedMemory programmedMemory;

	//Custom Command
	std::vector<uint8_t> customCmdBuffer;

	//Writing task
	std::ofstream fileWriteStream;
	uint32_t bytesWritten;
	uint32_t extendAddress;
	uint32_t writeAddress;
	std::string bufData;
	std::string bufLine;

	//Logging task
	bool enLog;
	bool enQuiet;
	std::ofstream fileLogStream;

	/***********************************************************************************
	*Function:    calcAndConvCheckSumHexLine
	*Description: calculate the checksum of a hex line
	*Parameters:  std::string input: hex line input
	*Returns:	  std::string
	***********************************************************************************/
	std::string calcAndConvCheckSumHexLine(std::string input);

	/***********************************************************************************
	*Function:    getDataOfProgrammedMemoryTxtFile
	*Description: get the programmed memory information from .txt file
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void getDataOfProgrammedMemoryTxtFile();

	/***********************************************************************************
	*Function:    getDataOfProgrammedMemoryHexFile
	*Description: get the programmed memory information from .hex file
	*Parameters:  -
	*Returns:	  -
	***********************************************************************************/
	void getDataOfProgrammedMemoryHexFile();

	/***********************************************************************************
	*Function:    writeVerbose
	*Description: write the bytes sent and received received
	*Parameters:  uint8_t *buf  : pointer to dataBuffer which data will be written
	                to the file
				  uint32_t bufSize	: number of bytes will be written to the file
	              uint8_t bracket : bracket type use for the verbose
	*Returns:	  -
	***********************************************************************************/
	void writeVerbose(std::vector<uint8_t> *buffer, uint32_t startIdx, uint32_t numOfBytes, uint8_t bracket);

};


