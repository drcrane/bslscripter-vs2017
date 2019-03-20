/*
==ScripterStream.cpp==

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

//Constructor
ScripterStream::ScripterStream(Util *util, bool cliMode)
{
	dataBlockRead.data.clear();
	dataBlockRead.startAddress = 0;

	fileScriptName.clear();
	line.clear();

	this->util = util;
	this->cliMode = cliMode;
}

ScripterStream::~ScripterStream()
{
	closeReadFile();
	closeWriteFile();
	closeLogFile();
}

void ScripterStream::setBufferProperties(uint16_t  maxBuffer, uint16_t header, uint16_t checkSum, uint16_t segmentSize)
{
	this->maxBufferSize = maxBuffer;
	this->numOfHeader = header;
	this->numOfCheckSum = checkSum;
	this->segmentSize = segmentSize;
}

uint32_t ScripterStream::retNumOfHeader()
{
	return numOfHeader;
}

uint32_t ScripterStream::retNumOfDataPerPacket()
{
	return maxBufferSize;
}

uint32_t ScripterStream::retNumberOfCheckSum()
{
	return numOfCheckSum;
}
 
bool ScripterStream::retCliMode()
{
	return cliMode;
}

void ScripterStream::setEnQuiet(bool enQuiet)
{
	this->enQuiet = enQuiet;
}	

bool ScripterStream::getEnQuiet()
{
	return enQuiet;
}

void ScripterStream::showHeaderFileStatus()
{
	ScripterStream::writeLogFile(showHeaderFile, false);
}

std::string ScripterStream::startScripter(char * ch)
{
	boost::filesystem::path fullPath(boost::filesystem::current_path());
	std::string path;
	std::string showFileName;

	date_time = boost::posix_time::second_clock::local_time();
	std::stringstream streamLocalTime;
	streamLocalTime << date_time;

	showHeaderFile  = "---------------------------------------------------------";
	showHeaderFile += "\nBSL Scripter ";
	showHeaderFile += std::to_string(Version::MAJOR) + ".";
	showHeaderFile += std::to_string(Version::MINOR) + ".";
	showHeaderFile += std::to_string(Version::BUG) + ".";
	showHeaderFile += std::to_string(Version::BUILD) + "\n";
	showHeaderFile += "\nPC software for BSL programming";
	showHeaderFile += "\n" + streamLocalTime.str();
 	showHeaderFile += "\n---------------------------------------------------------\n";

	if (cliMode)
	{
		return fullPath.string() + "/addition";
	}
	else
	{
		if (ch == nullptr)
		{
			ScripterStream::writeLogFile("Enter the name of file script : ", false);
			std::cin >> fileName;
		}
		else
		{
			fileName = (reinterpret_cast<char*>(ch));
		}
		std::string pathDir = fullPath.string();
		pathDir += "/addition";

		path = getThePathFile(fileName, pathDir);
		showFileName = path;
		std::replace(showFileName.begin(), showFileName.end(), '\\', '/');

		date_time = boost::posix_time::second_clock::local_time();
		std::stringstream streamLocalTime;
		streamLocalTime << date_time;

		showHeaderFile += ("Input file script is : " + showFileName) + "\n";

		ScripterStream::writeLogFile(showHeaderFile, false);

		fileScriptStream.open(path, std::ios::in);
		if (!fileScriptStream.good())
		{
			throw std::runtime_error("File Scripter is not available...");
		}
	}
	return path;
}

void ScripterStream::closeScriptFile()
{
	if (fileScriptStream.good())
	{
		fileScriptStream.close();
	}
}
 
bool ScripterStream::getLine()
{
	if (!fileScriptStream.eof())
	{
		std::getline(fileScriptStream, line);
		return true;
	}
	return false;
}
 
std::string ScripterStream::retStringCommand()
{
	return line;
}

ScripterReadStatus  ScripterStream::retScripterReadStatus()
{
	return scripterReadStatus;
}

ReadLineStatus ScripterStream::retReadLineStatus()
{
	return readLineStatus;
}

std::string ScripterStream::retPcAddressIntelHex()
{
	return pcAddressIntelHex;
}

bool ScripterStream::isTxtFile()
{
	return txtFile;
}

bool ScripterStream::isHexFile()
{
	return hexFile;
}

void ScripterStream::initReadCustomCommand(std::string filePath, std::string mainPathDir)
{
	txtFile = util->compareString(filePath.substr(filePath.size() - 4, 4), ".txt");
	if (!txtFile)
	{
		throw std::runtime_error("File type is not valid for Custom Command!");
	}
	fileName = getThePathFile(filePath, mainPathDir);
	std::string showFileName = fileName;
	std::replace(showFileName.begin(), showFileName.end(), '\\', '/');
	ScripterStream::writeLogFile(("Read Txt File for custom command  : " + showFileName), true);

	customCmdBuffer.clear();

	scripterReadStatus = ScripterReadStatus::OnGoing;

	fileReadStream.open(fileName, std::ios::in);
	if (!fileReadStream.good())
	{
		throw std::runtime_error("File is not available/accessed by other program...");
	}
}

void ScripterStream::readCustomCommand()
{
	std::string   tempLine;
	std::string   tempToken;

	if (!fileReadStream.eof())
	{
		std::getline(fileReadStream, tempLine);

		boost::char_separator<char> sep("\t\r ");
		boost::tokenizer<boost::char_separator<char>> tokens(tempLine, sep);

		for (const std::string& t : tokens)
		{
			tempToken = t;

			if ((tempToken == "q") || (tempToken == "Q"))
			{
				fileReadStream.close();
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
				readLineStatus = ReadLineStatus::QuitSign;
			}
			else
			{
				customCmdBuffer.push_back(util->convertHexStringToByte(tempToken));
				scripterReadStatus = ScripterReadStatus::OnGoing;

				if (customCmdBuffer.size() == maxBufferSize)
				{
					writeLogFile("Maximum buffer size  for custom command reached!", true);
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					readLineStatus = ReadLineStatus::QuitSign;
					return;
				}
			}
		}
	}
}

std::vector<uint8_t>* ScripterStream::retPtrCustomCmdBuffer()
{
	return &customCmdBuffer;
}

void ScripterStream::initReadFile(std::string filePath, std::string mainPathDir)
{
	hexFile = util->compareString(filePath.substr(filePath.size() - 4, 4), ".hex");
	txtFile = util->compareString(filePath.substr(filePath.size() - 4, 4), ".txt");

	if (!hexFile && !txtFile)
	{
		throw std::runtime_error("File type is not valid for BSL Scripter!");
	}

	std::replace(filePath.begin(), filePath.end(), '\\', '/');
	fileName = getThePathFile(filePath, mainPathDir);
	std::string showFileName = fileName;
	ScripterStream::writeLogFile(("Read Txt File  : " + showFileName), true);

	dataBlockRead.data.clear();
	leftOverE4xxHexFile.clear();
	bytesRead = 0;
	extLineAddress = 0;
	renewAddress = true;

	scripterReadStatus = ScripterReadStatus::OnGoing;
	readLineStatus = ReadLineStatus::NormalRead;

	fileReadStream.open(fileName, std::ios::in);
	if (!fileReadStream.good())
	{
		throw std::runtime_error("File is not available/accessed by other program...");
	}
}

void ScripterStream::getDataOfProgrammedMemory()
{
	if (isTxtFile())
	{
		getDataOfProgrammedMemoryTxtFile();
	}
	else
	{
		getDataOfProgrammedMemoryHexFile();
	}
}

void ScripterStream::getDataOfProgrammedMemoryHexFile()
{
	programmedMemory.address.clear();
	programmedMemory.sizeProgrammed.clear();
	programmedMemory.crcCheck.clear();

	std::string tempLine;
	std::string token;
	bool doTokens = true;
	bytesRead = 0;
	uint32_t segmentAddress = 0;
	util->resetCrc();

	while (!fileReadStream.eof())
	{
		hexLineByteCnt = 0;

		if (storeLine.size() != 0)
		{
			getHexLineParams(storeLine, bytesRead);
			if (hexLineByteCnt > 0)
			{
				for (uint32_t i = 0; i < hexLineByteCnt; i++)
				{
					token = storeLine.substr((9 + (i * 2)), 2);
					util->checkSumCalculate(util->convertHexStringToByte(token));
				}
				bytesRead += hexLineByteCnt;
				storeLine.clear();
			}
		}

		std::getline(fileReadStream, tempLine);
		getHexLineParams(tempLine, bytesRead);

		if (renewAddress)
		{
			renewAddress = false;

			if (bytesRead != 0)
			{
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
				segmentAddress = saveLastReadAddress;
				saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
				storeLine = tempLine;
				doTokens = false;
			}
			else
			{
				saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
				doTokens = true;
			}
		}
		else if (readLineStatus == ReadLineStatus::ExtAddressFound)
		{
			// no data here
			// the address will be updated along with the next data line
			renewAddress = true;
			// send the previous packet
			//
			if (bytesRead != 0)
			{
				segmentAddress = saveLastReadAddress;
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
			}
			doTokens = false;
		}
		else if (readLineStatus == ReadLineStatus::QuitSign)
		{
			fileReadStream.close();
			segmentAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			doTokens = false;
			break;
		}
		//check if the number of bytes already more than maxBufferSize
		else if ((dataBlockRead.data.size() + hexLineByteCnt) > maxBufferSize)
		{
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			dataBlockRead.startAddress = saveLastReadAddress;
			saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
			storeLine = tempLine;
			doTokens = false;
			return;
		}
		else
		{
			doTokens = true;
		}

		if (doTokens)
		{
			if (hexLineByteCnt < 1)
			{
				throw std::runtime_error("[ERROR_MESSAGE]Invalid number of bytes in Hex Line!");
			}
			for (uint16_t i = 0; i < hexLineByteCnt; i++)
			{
				token = tempLine.substr((9 + (i * 2)), 2);
				util->checkSumCalculate(util->convertHexStringToByte(token));
			} 
			bytesRead += hexLineByteCnt;

			if (bytesRead == segmentSize)
			{
				segmentAddress = saveLastReadAddress;
				saveLastReadAddress = dataBlockRead.startAddress + bytesRead;
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
				renewAddress = true;
			}
		}

		if (scripterReadStatus == ScripterReadStatus::DataBlockReady)
		{
			programmedMemory.address.push_back(util->convertUlongToString(segmentAddress, false));
			programmedMemory.sizeProgrammed.push_back(util->convertUlongToString(bytesRead, false));
			programmedMemory.crcCheck.push_back(util->convertUlongToString(util->retCrc(), false));

			bytesRead = 0;
			util->resetCrc();
			scripterReadStatus = ScripterReadStatus::OnGoing;
		}
 	}
}

void ScripterStream::getDataOfProgrammedMemoryTxtFile()
{
	std::string   tempLine;
	std::string   tempToken;

	util->resetCrc();
	while (!fileReadStream.eof())
	{
		std::getline(fileReadStream, tempLine);

		boost::char_separator<char> sep("\t\r ");
		boost::tokenizer<boost::char_separator<char>> tokens(tempLine, sep);

		for (const std::string& t : tokens)
		{
			tempToken = t;
			uint8_t pos = tempToken.find_first_of("@");
			if (pos == 0)
			{
				programmedMemory.address.push_back(tempLine.substr(1, tempLine.size()));
				if (programmedMemory.address.size() > 1)
				{
					programmedMemory.sizeProgrammed.push_back(util->convertUlongToString(bytesRead, true));
					programmedMemory.crcCheck.push_back(util->convertUlongToString(util->retCrc(), false));
					bytesRead = 0;
					util->resetCrc();
				}
			}
			//continue read the data on the same address
			else
			{
				if ((tempToken == "q") || (tempToken == "Q"))
				{
					//last size to be written to the vector
					programmedMemory.sizeProgrammed.push_back(util->convertUlongToString(bytesRead, true));
					programmedMemory.crcCheck.push_back(util->convertUlongToString(util->retCrc(), false));
					fileReadStream.close();
					return;
				}
				else
				{
					bytesRead++;
					util->checkSumCalculate(util->convertHexStringToByte(tempToken));
					if (bytesRead==segmentSize)
					{
						std::string tmp = programmedMemory.address.at(programmedMemory.address.size() - 1);
						uint32_t prevAddress = util->convertHexStringToUlong(tmp);
						prevAddress += segmentSize;
						programmedMemory.address.push_back(util->convertUlongToString(prevAddress,false));
						programmedMemory.sizeProgrammed.push_back(util->convertUlongToString(bytesRead, true));
						programmedMemory.crcCheck.push_back(util->convertUlongToString(util->retCrc(),false));
						bytesRead = 0;
						util->resetCrc();
					}
				}
			}
		}
	}
}

ProgrammedMemory* ScripterStream::retProgrammedMemory()
{
	return &programmedMemory;
}

void ScripterStream::clearDataBlock()
{
	dataBlockRead.data.clear();
	scripterReadStatus = ScripterReadStatus::OnGoing;
}

void ScripterStream::getHexLineParams(std::string line, uint32_t actualSize)
{
	cs = 0;

	//Check start code
	std::string tmp = line.substr(0, 1);
	if (!util->compareString(line.substr(0, 1), ":"))
	{
		throw std::runtime_error("[ERROR_MESSAGE]Start code of the Hex file is not present!");
	}

	//Get byte count
	tmp = line.substr(1, 2);
	hexLineByteCnt = util->convertHexStringToUlong(tmp);
	cs += (uint8_t)hexLineByteCnt;

	//Get Address
	tmp = line.substr(3, 4);
	hexLineAddress = util->convertHexStringToUlong(tmp);
	cs += (uint8_t)(util->convertHexStringToByte(tmp.substr(0, 2)));
	cs += (uint8_t)(util->convertHexStringToByte(tmp.substr(2, 2)));

	tmp = line.substr(7, 2);
	
	if (util->compareString(tmp, "04"))
	{
		readLineStatus = ReadLineStatus::ExtAddressFound;
		extLineAddress = util->convertHexStringToUlong(line.substr(9, 4));
	}
	else if (util->compareString(tmp, "00"))
	{
		readLineStatus = ReadLineStatus::NormalRead;
		//check if the address is jumping
		uint32_t actAddress = hexLineAddress + (extLineAddress << 16);
		if (actAddress > (actualSize+ saveLastReadAddress))
		{
			renewAddress = true;
		}
	}
	else if (util->compareString(tmp, "01"))
	{
		readLineStatus = ReadLineStatus::QuitSign;
		renewAddress = false;
	}
	else if (util->compareString(tmp, "03"))
	{
		intelHex03detect = true;
		pcAddressIntelHex = line.substr(9, 8);
	}
	else
	{
		throw std::runtime_error("[ERROR_MESSAGE]Record type is not valid for the Scripter!");
	}
	cs += (uint8_t)(util->convertHexStringToByte(tmp));
}

void ScripterStream::resetIntelHex03detect()
{
	intelHex03detect = false;
}

bool ScripterStream::retIntelHex03detect()
{
	return intelHex03detect;
}
	
void ScripterStream::readHexFile()
{
	std::string tempLine;
	std::string token;
	uint8_t convertToken;
	uint32_t idxOnString = 0;

	hexLineByteCnt = 0;

	if (!fileReadStream.eof())
	{
		if (storeLine.size() != 0)
		{
			getHexLineParams(storeLine, dataBlockRead.data.size());
			if (hexLineByteCnt > 0)
			{
				for (uint16_t i = 0; i < hexLineByteCnt; i++)
				{
					idxOnString = 9 + (i * 2);
					token = storeLine.substr(idxOnString, 2);
					convertToken = util->convertHexStringToByte(token);
					dataBlockRead.data.push_back(convertToken);
					cs += (uint32_t)convertToken;
					bytesRead++;
				}

				token = storeLine.substr(idxOnString + 2, 2);

				uint8_t csCnt = -((uint8_t)cs);
				uint8_t csLine = util->convertHexStringToByte(token);
				if (csCnt != csLine)
				{
					throw std::runtime_error("[ERROR_MESSAGE]Invalid check sum on the hex line!");
				}
				storeLine.clear();

				if (dataBlockRead.data.size() == maxBufferSize)
				{
					dataBlockRead.startAddress = saveLastReadAddress;
					saveLastReadAddress = dataBlockRead.startAddress + dataBlockRead.data.size();
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					return;
				}
			}
		}

		std::getline(fileReadStream, tempLine);
		getHexLineParams(tempLine, dataBlockRead.data.size());

		if (readLineStatus == ReadLineStatus::ExtAddressFound)
		{
			// no data here
			// the address will be updated along with the next data line
			renewAddress = true;
			// send the previous packet
			if (dataBlockRead.data.size() != 0)
			{
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
				dataBlockRead.startAddress = saveLastReadAddress;
				saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
			}
			return;
		}
		
		if(renewAddress)
		{
			renewAddress = false;

			if (dataBlockRead.data.size() != 0)
			{
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
				dataBlockRead.startAddress = saveLastReadAddress;
				saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
				storeLine = tempLine;
				return;
			}
			saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
		}
		else if (intelHex03detect)
		{
			dataBlockRead.startAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}
		else if (readLineStatus == ReadLineStatus::QuitSign)
		{
			fileReadStream.close();
			dataBlockRead.startAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}
		//check if the number of bytes already more than maxBufferSize
		else if ((dataBlockRead.data.size()+hexLineByteCnt) > maxBufferSize)
		{
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			dataBlockRead.startAddress = saveLastReadAddress;
			saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
			storeLine = tempLine;
			return;
		}

		for (uint16_t i = 0; i < hexLineByteCnt; i++)
		{
			idxOnString = 9 + (i * 2);
			token = tempLine.substr(idxOnString, 2);
			convertToken = util->convertHexStringToByte(token);
			dataBlockRead.data.push_back(convertToken);
			cs += (uint32_t)convertToken;
			bytesRead++;
		}

		token = tempLine.substr(idxOnString+2, 2);

		uint8_t csCnt = -(uint8_t)cs;
		uint8_t csLine = util->convertHexStringToByte(token);

		if (csCnt != csLine)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Invalid check sum on the hex line!");
		}

		if (dataBlockRead.data.size() == maxBufferSize)
		{
			dataBlockRead.startAddress = saveLastReadAddress;
			saveLastReadAddress = dataBlockRead.startAddress + dataBlockRead.data.size();
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			renewAddress = true;
		}
	}
}

void ScripterStream::readHexFileE4xx()
{
	std::string tempLine;
	std::string token;
	uint8_t convertToken;
	uint32_t idxOnString = 0;
	static uint16_t numOfLines = 0;
	static uint32_t idSector = 0; // this variable will calculate new sector will be programmed
	uint32_t newIdSector = 0;

	hexLineByteCnt = 0;

	if (!fileReadStream.eof())
	{
		if (leftOverE4xxHexFile.size() != 0)
		{
			for (uint16_t i = 0; i < leftOverE4xxHexFile.size(); i++)
			{
				dataBlockRead.data.push_back(leftOverE4xxHexFile.at(i));
			}
			bytesRead += leftOverE4xxHexFile.size();
			leftOverE4xxHexFile.clear();
		}

		if (storeLine.size() != 0)
		{
			getHexLineParams(storeLine, dataBlockRead.data.size());
			if (hexLineByteCnt > 0)
			{
				for (uint16_t i = 0; i < hexLineByteCnt; i++)
				{
					idxOnString = 9 + (i * 2);
					token = storeLine.substr(idxOnString, 2);
					convertToken = util->convertHexStringToByte(token);
					dataBlockRead.data.push_back(convertToken);
					cs += (uint32_t)convertToken;
					bytesRead++;
				}

				token = storeLine.substr(idxOnString + 2, 2);

				uint8_t csCnt = -((uint8_t)cs);
				uint8_t csLine = util->convertHexStringToByte(token);
				if (csCnt != csLine)
				{
					throw std::runtime_error("[ERROR_MESSAGE]Invalid check sum on the hex line!");
				}
				storeLine.clear();

				if (dataBlockRead.data.size() == maxBufferSize)
				{
					dataBlockRead.startAddress = saveLastReadAddress;
					saveLastReadAddress = dataBlockRead.startAddress + dataBlockRead.data.size();
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					return;
				}
			}
			storeLine.clear();
		}

		std::getline(fileReadStream, tempLine);
		getHexLineParams(tempLine, dataBlockRead.data.size());

		if (readLineStatus == ReadLineStatus::ExtAddressFound)
		{
			// no data here
			// the address will be updated along with the next data line
			renewAddress = true;
			// send the previous packet
			if (dataBlockRead.data.size() > 0)
			{
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
				dataBlockRead.startAddress = saveLastReadAddress;
			}
			return;
		}

		if (renewAddress)
		{
			renewAddress = false;
			//save the very first address of the firmware image
			if (numOfLines == 0)
			{
				saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
				dataBlockRead.startAddress = saveLastReadAddress;
				idSector = saveLastReadAddress / segmentSize;
				newIdSector = idSector;
			}
			else
			{
				//new address is found
				newIdSector = (hexLineAddress + (extLineAddress << 16)) / segmentSize;

				if (newIdSector > idSector)
				{
					//send the present datablock
					if (dataBlockRead.data.size() != 0)
					{
						dataBlockRead.startAddress = saveLastReadAddress;
						saveLastReadAddress = (hexLineAddress + (extLineAddress << 16));
						scripterReadStatus = ScripterReadStatus::DataBlockReady;
						idSector = newIdSector;
						storeLine = tempLine;
						return;
					}
					idSector = newIdSector;
					saveLastReadAddress = (hexLineAddress + (extLineAddress << 16));
					
				}
				else
				{
					scripterReadStatus = ScripterReadStatus::OnGoing;
					uint32_t addressFillWithFF = hexLineAddress + (extLineAddress << 16);
					//filled in the gap between latest address in dataBlokRead
					//   and the new address with 0xFF
					if (addressFillWithFF > 0)
					{
						for (uint32_t j = (saveLastReadAddress + dataBlockRead.data.size());
							j < addressFillWithFF;
							j++)
						{
							dataBlockRead.data.push_back(0xFF);
						}
					}
					else
					{
						throw std::runtime_error("[ERROR_MESSAGE]Invalid firmware image!");
					}
				}
			}
		}
		else if (intelHex03detect)
		{
			dataBlockRead.startAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}
		else if (readLineStatus == ReadLineStatus::QuitSign)
		{
			fileReadStream.close();
			dataBlockRead.startAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}

		//check if the number of bytes already more than maxBufferSize
		if ((saveLastReadAddress+dataBlockRead.data.size() + hexLineByteCnt) > (segmentSize*(idSector+1)))
		{
			leftOverE4xxHexFile.clear();
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			dataBlockRead.startAddress = saveLastReadAddress;
			//Store the leftover bytes to be sent in next cycle
			uint16_t numberOfLeftOverBytes = (saveLastReadAddress + dataBlockRead.data.size() + hexLineByteCnt) - (segmentSize*(idSector + 1));
			for (uint16_t i = 0; i < numberOfLeftOverBytes; i++)
			{
				idxOnString = 9 + (hexLineByteCnt - numberOfLeftOverBytes) +(i* 2);
				token = tempLine.substr(idxOnString, 2);
				convertToken = util->convertHexStringToByte(token);
				leftOverE4xxHexFile.push_back(convertToken);
				//bytesRead++;
			}

			dataBlockRead.startAddress = saveLastReadAddress;
			saveLastReadAddress = segmentSize*(idSector + 1);
			idSector = saveLastReadAddress / segmentSize;

			for (uint16_t i = 0; i < (hexLineByteCnt - numberOfLeftOverBytes); i++)
			{
				idxOnString = 9 + (i * 2);
				token = tempLine.substr(idxOnString, 2);
				convertToken = util->convertHexStringToByte(token);
				dataBlockRead.data.push_back(convertToken);
				cs += (uint32_t)convertToken;
				bytesRead++;
			}
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}

		//Reading normally
		for (uint16_t i = 0; i < hexLineByteCnt; i++)
		{
			idxOnString = 9 + (i * 2);
			token = tempLine.substr(idxOnString, 2);
			convertToken = util->convertHexStringToByte(token);
			dataBlockRead.data.push_back(convertToken);
			cs += (uint32_t)convertToken;
			bytesRead++;
		}

		token = tempLine.substr(idxOnString + 2, 2);

		uint8_t csCnt = -(uint8_t)cs;
		uint8_t csLine = util->convertHexStringToByte(token);

		if (csCnt != csLine)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Invalid check sum on the hex line!");
		}

		numOfLines++;
	}
}

void ScripterStream::readHexFileE4xxUsbDfu()
{
	std::string tempLine;
	std::string token;
	uint8_t convertToken;
	uint32_t idxOnString = 0;
	static uint16_t numOfLines = 0;
	static uint32_t idSector = 0; // this variable will calculate new sector will be programmed
	uint32_t newIdSector = 0;

	hexLineByteCnt = 0;

	if (!fileReadStream.eof())
	{
		if (leftOverE4xxHexFile.size() != 0)
		{
			for (uint16_t i = 0; i < leftOverE4xxHexFile.size(); i++)
			{
				dataBlockRead.data.push_back(leftOverE4xxHexFile.at(i));
			}
			bytesRead += leftOverE4xxHexFile.size();
			leftOverE4xxHexFile.clear();
		}

		if (storeLine.size() != 0)
		{
			getHexLineParams(storeLine, dataBlockRead.data.size());
			if (hexLineByteCnt > 0)
			{
				for (uint16_t i = 0; i < hexLineByteCnt; i++)
				{
					idxOnString = 9 + (i * 2);
					token = storeLine.substr(idxOnString, 2);
					convertToken = util->convertHexStringToByte(token);
					dataBlockRead.data.push_back(convertToken);
					cs += (uint32_t)convertToken;
					bytesRead++;
				}

				token = storeLine.substr(idxOnString + 2, 2);

				uint8_t csCnt = -((uint8_t)cs);
				uint8_t csLine = util->convertHexStringToByte(token);
				if (csCnt != csLine)
				{
					throw std::runtime_error("[ERROR_MESSAGE]Invalid check sum on the hex line!");
				}
				storeLine.clear();

				if (dataBlockRead.data.size() == maxBufferSize)
				{
					dataBlockRead.startAddress = saveLastReadAddress;
					saveLastReadAddress = dataBlockRead.startAddress + dataBlockRead.data.size();
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					return;
				}
			}
			storeLine.clear();
		}

		std::getline(fileReadStream, tempLine);
		getHexLineParams(tempLine, dataBlockRead.data.size());

		if (readLineStatus == ReadLineStatus::ExtAddressFound)
		{
			// no data here
			// the address will be updated along with the next data line
			renewAddress = true;
			// send the previous packet
			if (dataBlockRead.data.size() > 0)
			{
				scripterReadStatus = ScripterReadStatus::DataBlockReady;
				dataBlockRead.startAddress = saveLastReadAddress;
			}
			return;
		}

		if (renewAddress)
		{
			renewAddress = false;
			//save the very first address of the firmware image
			if (numOfLines == 0)
			{
				saveLastReadAddress = hexLineAddress + (extLineAddress << 16);
				dataBlockRead.startAddress = saveLastReadAddress;
				idSector = saveLastReadAddress / maxBufferSize;
				newIdSector = idSector;
			}
			else
			{
				//new address is found
				newIdSector = (hexLineAddress + (extLineAddress << 16)) / maxBufferSize;

				if (newIdSector > idSector)
				{
					//send the present datablock
					if (dataBlockRead.data.size() != 0)
					{
						dataBlockRead.startAddress = saveLastReadAddress;
						saveLastReadAddress = (hexLineAddress + (extLineAddress << 16));
						scripterReadStatus = ScripterReadStatus::DataBlockReady;
						idSector = newIdSector;
						storeLine = tempLine;
						return;
					}
					idSector = newIdSector;
					saveLastReadAddress = (hexLineAddress + (extLineAddress << 16));

				}
				else
				{
					scripterReadStatus = ScripterReadStatus::OnGoing;
					uint32_t addressFillWithFF = hexLineAddress + (extLineAddress << 16);
					//filled in the gap between latest address in dataBlokRead
					//   and the new address with 0xFF
					if (addressFillWithFF > 0)
					{
						for (uint32_t j = (saveLastReadAddress + dataBlockRead.data.size());
							j < addressFillWithFF;
							j++)
						{
							dataBlockRead.data.push_back(0xFF);
						}
					}
					else
					{
						throw std::runtime_error("[ERROR_MESSAGE]Invalid firmware image!");
					}
				}
			}
		}
		else if (intelHex03detect)
		{
			dataBlockRead.startAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}
		else if (readLineStatus == ReadLineStatus::QuitSign)
		{
			fileReadStream.close();
			dataBlockRead.startAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}

		//check if the number of bytes already more than maxBufferSize
		if ((saveLastReadAddress + dataBlockRead.data.size() + hexLineByteCnt) > (maxBufferSize*(idSector + 1)))
		{
			leftOverE4xxHexFile.clear();
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			dataBlockRead.startAddress = saveLastReadAddress;
			//Store the leftover bytes to be sent in next cycle
			uint16_t numberOfLeftOverBytes = (saveLastReadAddress + dataBlockRead.data.size() + hexLineByteCnt) - (maxBufferSize*(idSector + 1));
			for (uint16_t i = 0; i < numberOfLeftOverBytes; i++)
			{
				idxOnString = 9 + (hexLineByteCnt - numberOfLeftOverBytes) + (i * 2);
				token = tempLine.substr(idxOnString, 2);
				convertToken = util->convertHexStringToByte(token);
				leftOverE4xxHexFile.push_back(convertToken);
				//bytesRead++;
			}

			dataBlockRead.startAddress = saveLastReadAddress;
			saveLastReadAddress = maxBufferSize*(idSector + 1);
			idSector = saveLastReadAddress / maxBufferSize;

			for (uint16_t i = 0; i < (hexLineByteCnt - numberOfLeftOverBytes); i++)
			{
				idxOnString = 9 + (i * 2);
				token = tempLine.substr(idxOnString, 2);
				convertToken = util->convertHexStringToByte(token);
				dataBlockRead.data.push_back(convertToken);
				cs += (uint32_t)convertToken;
				bytesRead++;
			}
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}

		//Reading normally
		for (uint16_t i = 0; i < hexLineByteCnt; i++)
		{
			idxOnString = 9 + (i * 2);
			token = tempLine.substr(idxOnString, 2);
			convertToken = util->convertHexStringToByte(token);
			dataBlockRead.data.push_back(convertToken);
			cs += (uint32_t)convertToken;
			bytesRead++;
		}

		token = tempLine.substr(idxOnString + 2, 2);

		uint8_t csCnt = -(uint8_t)cs;
		uint8_t csLine = util->convertHexStringToByte(token);

		if (csCnt != csLine)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Invalid check sum on the hex line!");
		}

		numOfLines++;
	}
}

void ScripterStream::readHexFileE4xxEthernet()
{
	std::string tempLine;
	std::string token;
	uint8_t convertToken;
	uint32_t idxOnString = 0;
	static uint16_t numOfLines = 0;

	hexLineByteCnt = 0;

	if (!fileReadStream.eof())
	{
		std::getline(fileReadStream, tempLine);
		getHexLineParams(tempLine, dataBlockRead.data.size());

		if (readLineStatus == ReadLineStatus::ExtAddressFound)
		{
			// no data here
			// the address will be updated along with the next data line
			renewAddress = true;
			return;
		}
		else if ((hexLineAddress + (extLineAddress << 16)) > (dataBlockRead.startAddress + dataBlockRead.data.size()))
		{
			renewAddress = true;
		}

		if (renewAddress)
		{
			renewAddress = false;
			//save the very first address of the firmware image
			if (numOfLines == 0)
			{
				dataBlockRead.startAddress = hexLineAddress + (extLineAddress << 16);
			}
			else
			{
				scripterReadStatus = ScripterReadStatus::OnGoing;
				uint32_t addressFillWithFF = hexLineAddress + (extLineAddress << 16);
				//filled in the gap between latest address in dataBlokRead
				//   and the new address with 0xFF
				if (addressFillWithFF < ProgramMemorySize::MSP432E)
				{
					for (uint32_t j = (dataBlockRead.startAddress + dataBlockRead.data.size());
						j < addressFillWithFF;
						j++)
					{
						dataBlockRead.data.push_back(0xFF);
					}
				}
				else
				{
					throw std::runtime_error("[ERROR_MESSAGE]Invalid firmware image!");
				}
			}
		}
		else if (intelHex03detect)
		{
			dataBlockRead.startAddress = saveLastReadAddress;
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}
		else if (readLineStatus == ReadLineStatus::QuitSign)
		{
			fileReadStream.close();
			scripterReadStatus = ScripterReadStatus::DataBlockReady;
			return;
		}

		//Reading normally
		for (uint16_t i = 0; i < hexLineByteCnt; i++)
		{
			idxOnString = 9 + (i * 2);
			token = tempLine.substr(idxOnString, 2);
			convertToken = util->convertHexStringToByte(token);
			dataBlockRead.data.push_back(convertToken);
			cs += (uint32_t)convertToken;
			bytesRead++;
		}

		token = tempLine.substr(idxOnString + 2, 2);

		uint8_t csCnt = -(uint8_t)cs;
		uint8_t csLine = util->convertHexStringToByte(token);

		if (csCnt != csLine)
		{
			throw std::runtime_error("[ERROR_MESSAGE]Invalid check sum on the hex line!");
		}

		numOfLines++;
	}
}

void ScripterStream::readTxtFile()
{
	std::string   tempLine;
	std::string   tempToken;

	if (!fileReadStream.eof())
	{
		std::getline(fileReadStream, tempLine);

		boost::char_separator<char> sep("\t\r ");
		boost::tokenizer<boost::char_separator<char>> tokens(tempLine, sep);

		for (const std::string& t : tokens)
		{
			tempToken = t;
			uint8_t pos = tempToken.find_first_of("@");
			if (pos == 0)
			{
				readLineStatus = ReadLineStatus::AddressFound;
				tempToken.erase(0, 1);
				saveLastReadAddress = std::stoul(tempToken, nullptr, 16);

				if (dataBlockRead.data.size() > 0)
				{
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					return;
				}
			}
			//continue read the data on the same address
			else
			{
				if ((tempToken == "q") || (tempToken == "Q"))
				{
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					readLineStatus = ReadLineStatus::QuitSign;
					fileReadStream.close();
					return;
				}
				else
				{
					if (readLineStatus == ReadLineStatus::AddressFound)
					{
						dataBlockRead.startAddress = saveLastReadAddress;
						readLineStatus = ReadLineStatus::NormalRead;
					}

					dataBlockRead.data.push_back(util->convertHexStringToByte(tempToken));
					bytesRead++;
					scripterReadStatus = ScripterReadStatus::OnGoing;

					if (dataBlockRead.data.size() == maxBufferSize)
					{
						saveLastReadAddress = dataBlockRead.data.size() + dataBlockRead.startAddress;
						readLineStatus = ReadLineStatus::AddressFound;
						scripterReadStatus = ScripterReadStatus::DataBlockReady;
						return;
					}
				}
			}
		}
	}
}

void ScripterStream::readTxtFileE4xx()
{
	std::string   tempLine;
	std::string   tempToken;
	static uint16_t numOfLines = 0;
	static uint16_t idSector = 0; // this variable will calculate new sector will be programmed
	uint32_t newIdSector = 0;

	if (!fileReadStream.eof())
	{
		std::getline(fileReadStream, tempLine);

		boost::char_separator<char> sep("\t\r ");
		boost::tokenizer<boost::char_separator<char>> tokens(tempLine, sep);

		for (const std::string& t : tokens)
		{
			tempToken = t;
			uint8_t pos = tempToken.find_first_of("@");
			if (pos == 0)
			{
				readLineStatus = ReadLineStatus::AddressFound;
				tempToken.erase(0, 1);

				//save the very first address of the firmware image
				if (numOfLines == 0)
				{
					saveLastReadAddress = util->convertHexStringToUlong(tempToken);
					dataBlockRead.startAddress = saveLastReadAddress;
					idSector = saveLastReadAddress / segmentSize;
					newIdSector = idSector;
				}
				else
				{
					//new address is found
					newIdSector = std::stoul(tempToken, nullptr, 16) / segmentSize;

					if (newIdSector > idSector)
					{
						//send the present datablock
						dataBlockRead.startAddress = saveLastReadAddress;
						scripterReadStatus = ScripterReadStatus::DataBlockReady;
						saveLastReadAddress = util->convertHexStringToUlong(tempToken);
						idSector = newIdSector;
						return;
					}
					else
					{
						scripterReadStatus = ScripterReadStatus::OnGoing;
						uint32_t addressFillWithFF = util->convertHexStringToUlong(tempToken);
						//filled in the gap between latest address in dataBlokRead
						//   and the new address with 0xFF
						if (addressFillWithFF < ProgramMemorySize::MSP432E)
						{
							for (uint32_t j = (saveLastReadAddress + dataBlockRead.data.size());
								j < addressFillWithFF;
								j++)
							{
								dataBlockRead.data.push_back(0xFF);
							}
						}
						else
						{
							throw std::runtime_error("[ERROR_MESSAGE]Invalid firmware image!");
						}
					}
				}
			}
			//continue read the data that starts on same sector
			else
			{
				if ((tempToken == "q") || (tempToken == "Q"))
				{
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					readLineStatus = ReadLineStatus::QuitSign;
					dataBlockRead.startAddress = saveLastReadAddress;
					fileReadStream.close();
					idSector = 0;
					numOfLines = 0;
					return;
				}
				else
				{
					//Reading the data bytes
					scripterReadStatus = ScripterReadStatus::OnGoing;
					dataBlockRead.data.push_back(util->convertHexStringToByte(tempToken));
					bytesRead++;

					//Check if the dataBlockRead already full one  segment size
					if (((saveLastReadAddress + dataBlockRead.data.size()) % segmentSize) == 0)
					{
						scripterReadStatus = ScripterReadStatus::DataBlockReady;
						dataBlockRead.startAddress = saveLastReadAddress;
						//The new address has to be set for next dataBlock
						saveLastReadAddress += dataBlockRead.data.size();
						idSector++;
						return;
					}
				}
			}
		}
		numOfLines++;
	}
}

void ScripterStream::readTxtFileE4xxEthernet()
{
	std::string   tempLine;
	std::string   tempToken;
	static uint16_t numOfLines = 0;

	while (!fileReadStream.eof())
	{
		std::getline(fileReadStream, tempLine);

		boost::char_separator<char> sep("\t\r ");
		boost::tokenizer<boost::char_separator<char>> tokens(tempLine, sep);

		for (const std::string& t : tokens)
		{
			scripterReadStatus = ScripterReadStatus::OnGoing;
			tempToken = t;
			uint8_t pos = tempToken.find_first_of("@");
			//padding is required when:
			//   the first address is not @0
			//   new address is found, and there is a gap between new address and the last address
			//   in the dataBlockRead
			if (pos == 0)
			{
				tempToken.erase(0, 1);
				//save the very first address of the firmware image
				if (numOfLines == 0)
				{
					dataBlockRead.data.clear();
					dataBlockRead.startAddress = util->convertHexStringToUlong(tempToken);
				}
				else
				{
					saveLastReadAddress = util->convertHexStringToUlong(tempToken);
					if (saveLastReadAddress < ProgramMemorySize::MSP432E)
					{
						for (uint32_t j = (dataBlockRead.data.size() + dataBlockRead.startAddress); j < saveLastReadAddress; j++)
						{
							dataBlockRead.data.push_back(0xFF);
						}
					}
				}
			}
			//continue read the data that starts on same sector
			else
			{
				if ((tempToken == "q") || (tempToken == "Q"))
				{
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					readLineStatus = ReadLineStatus::QuitSign;
					fileReadStream.close();
					return;
				}
				else
				{
					//Reading the data bytes
					scripterReadStatus = ScripterReadStatus::OnGoing;
					dataBlockRead.data.push_back(util->convertHexStringToByte(tempToken));
					bytesRead++;
				}
			}
		}
		numOfLines++;
	}
}

void ScripterStream::readTxtFileE4xxUsbDfu()
{
	std::string   tempLine;
	std::string   tempToken;
	static uint16_t numOfLines = 0;
	static uint16_t idSector = 0; // this variable will calculate new sector will be programmed
	uint32_t newIdSector = 0;

	if (!fileReadStream.eof())
	{
		std::getline(fileReadStream, tempLine);

		boost::char_separator<char> sep("\t\r ");
		boost::tokenizer<boost::char_separator<char>> tokens(tempLine, sep);

		for (const std::string& t : tokens)
		{
			tempToken = t;
			uint8_t pos = tempToken.find_first_of("@");
			if (pos == 0)
			{
				readLineStatus = ReadLineStatus::AddressFound;
				tempToken.erase(0, 1);

				//save the very first address of the firmware image
				if (numOfLines == 0)
				{
					saveLastReadAddress = util->convertHexStringToUlong(tempToken);
					dataBlockRead.startAddress = saveLastReadAddress;
					idSector = saveLastReadAddress / maxBufferSize;
					newIdSector = idSector;
				}
				else
				{
					//new address is found
					newIdSector = std::stoul(tempToken, nullptr, 16) / maxBufferSize;

					if (newIdSector > idSector)
					{
						//send the present datablock
						dataBlockRead.startAddress = saveLastReadAddress;
						scripterReadStatus = ScripterReadStatus::DataBlockReady;
						saveLastReadAddress = util->convertHexStringToUlong(tempToken);
						idSector = newIdSector;
						return;
					}
					else
					{
						scripterReadStatus = ScripterReadStatus::OnGoing;
						uint32_t addressFillWithFF = util->convertHexStringToUlong(tempToken);
						//filled in the gap between latest address in dataBlokRead
						//   and the new address with 0xFF
						if (addressFillWithFF < ProgramMemorySize::MSP432E)
						{
							for (uint32_t j = (saveLastReadAddress + dataBlockRead.data.size());
								j < addressFillWithFF;
								j++)
							{
								dataBlockRead.data.push_back(0xFF);
							}
						}
						else
						{
							throw std::runtime_error("[ERROR_MESSAGE]Invalid firmware image!");
						}
					}
				}
			}
			//continue read the data that starts on same sector
			else
			{
				if ((tempToken == "q") || (tempToken == "Q"))
				{
					scripterReadStatus = ScripterReadStatus::DataBlockReady;
					readLineStatus = ReadLineStatus::QuitSign;
					dataBlockRead.startAddress = saveLastReadAddress;
					fileReadStream.close();
					idSector = 0;
					numOfLines = 0;
					return;
				}
				else
				{
					//Reading the data bytes
					scripterReadStatus = ScripterReadStatus::OnGoing;
					dataBlockRead.data.push_back(util->convertHexStringToByte(tempToken));
					bytesRead++;

					//Check if the dataBlockRead already full one  segment size
					if (((saveLastReadAddress + dataBlockRead.data.size()) % maxBufferSize) == 0)
					{
						scripterReadStatus = ScripterReadStatus::DataBlockReady;
						dataBlockRead.startAddress = saveLastReadAddress;
						//The new address has to be set for next dataBlock
						saveLastReadAddress += dataBlockRead.data.size();
						idSector++;
						return;
					}
				}
			}
		}
		numOfLines++;
	}
}

DataBlock* ScripterStream::retDataBlockRead()
{
	return &dataBlockRead;
}
 
void ScripterStream::closeReadFile()
{
	if (fileReadStream.good())
	{
		fileReadStream.close();
	}
}

std::string ScripterStream::calcAndConvCheckSumHexLine(std::string input)
{
	cs = 0;
	if ((input.size() % 2) == 0)
	{
		throw std::runtime_error("Hex Line is not valid, no check sum can be calculated!");
	}

	for (uint16_t i = 0; i < ((input.size() - 1) / 2); i++)
	{
		cs += util->convertHexStringToByte(input.substr(2 * i + 1, 2));
	}
	uint8_t csCnt = -(uint8_t)cs;
	std::string tmp = util->convertByteToString(csCnt, false);
	boost::to_upper(tmp);
	return tmp;
}

void ScripterStream::initWriteFile(std::string filePath, std::string address, std::string mainPathDir)
{
	hexFile = util->compareString(filePath.substr(filePath.size() - 4, 4), ".hex");
	txtFile = util->compareString(filePath.substr(filePath.size() - 4, 4), ".txt");

	if (!hexFile && !txtFile)
	{
		throw std::runtime_error("File type is not valid for BSL Scripter!");
	}

	fileName = getThePathFile(filePath, mainPathDir);
	std::string showFileName = fileName;
	std::replace(showFileName.begin(), showFileName.end(), '\\', '/');
	ScripterStream::writeLogFile(("Write Txt File : " + showFileName), true);
	fileWriteStream.open(fileName, std::ios::out);

	if (util->compareString(address.substr(0, 2), "0x"))
	{
		address = address.substr(2, address.size());
	}

	boost::to_upper(address);
	if (txtFile)
	{
		fileWriteStream << "@" << address << "\n";
	}
	else
	{
		bufLine.clear();
		writeAddress = util->convertHexStringToUlong(address.substr(address.size() - 4));
		extendAddress = 0;
		if (util->convertHexStringToUlong(address) > 0xFFFF)
		{
			extendAddress = util->convertHexStringToUlong(address.substr(0, (address.size() - 4))) - 0xFFFF;
			bufLine = ":02000004" + util->fillInZero(address.substr(0, (address.size() - 4)), 4);
			bufLine += calcAndConvCheckSumHexLine(bufLine);
			fileWriteStream << bufLine << std::endl;
		}
	}
	if (!fileWriteStream.good())
	{
		throw std::runtime_error("File to write the file could not be created...");
	}
}		  
  
void ScripterStream::writeHexFile(std::vector<uint8_t> *buf, uint32_t sizeBuf)
{
	uint32_t bytesPerPacket = maxBufferSize + numOfHeader + numOfCheckSum;
	float packets = (float)(sizeBuf) / (bytesPerPacket);
	uint32_t numOfPacket = (uint16_t)(ceil(packets));

	bytesWritten = 0;
	bufData.clear();

	for (uint32_t i = 0; i < numOfPacket; i++)
	{
		uint32_t start = (i*bytesPerPacket) + numOfHeader;
		uint32_t stop;

		bool highAddress = false;
		uint32_t cntHighAddress=0;

		if (i == (numOfPacket - 1))
		{
			stop = sizeBuf - numOfCheckSum;
		}
		else
		{
			stop = (bytesPerPacket * (i + 1)) - numOfCheckSum;
		}

		for (uint32_t k = start; k < stop; k++)
		{
			std::string tempString = util->convertByteToString((*buf).at(k), false);
			boost::to_upper(tempString);
			bufData += tempString;
			bytesWritten++;
			if (highAddress)
			{
				cntHighAddress++;
			}

			uint16_t indexOfBytesInLine = bufData.size() / 2;

			if ((indexOfBytesInLine + writeAddress - 1) >= 0xFFFF)
			{
				bufLine = ":" + util->convertByteToString(bufData.size() / 2, false) + util->fillInZero(util->convertUlongToString(writeAddress, false), 4) + "00" + bufData;
				bufLine += calcAndConvCheckSumHexLine(bufLine);
				fileWriteStream << bufLine << std::endl;
				bufData.clear();
				bufLine.clear();
				writeAddress = 0;

				std::string tmp = util->convertUlongToString((extendAddress + 0x10000), false);
				bufLine = ":02000004" + util->fillInZero(tmp.substr(0, (tmp.size() - 4)), 4);
				bufLine += calcAndConvCheckSumHexLine(bufLine);
				fileWriteStream << bufLine << std::endl;
				bufLine.clear();
				bufData.clear();
				highAddress = true;
			}
			else if (highAddress && (cntHighAddress % 16 == 0))
			{
				bufLine = ":" + util->convertByteToString(bufData.size() / 2, false) + util->fillInZero(util->convertUlongToString(writeAddress, false), 4) + "00" + bufData;
				bufLine += calcAndConvCheckSumHexLine(bufLine);
				fileWriteStream << bufLine << std::endl;
				bufData.clear();
				bufLine.clear();
				writeAddress += 16;
			}
			else if (!highAddress && (bytesWritten % 16 == 0) )
			{
				bufLine = ":" + util->convertByteToString(bufData.size() / 2, false) + util->fillInZero(util->convertUlongToString(writeAddress, false), 4) + "00" + bufData;
				bufLine += calcAndConvCheckSumHexLine(bufLine);
				fileWriteStream << bufLine << std::endl;
				bufData.clear();
				bufLine.clear();
				writeAddress += 16;
			}
		}
	}
}

void ScripterStream::writeTxtFile(std::vector<uint8_t> *buf, uint32_t sizeBuf)
{
	uint32_t bytesPerPacket = maxBufferSize + numOfHeader + numOfCheckSum;
	float packets = (float)(sizeBuf) / (bytesPerPacket);
	uint32_t numOfPacket = (uint16_t)(ceil(packets));

	bytesWritten = 0;

	for (uint32_t i = 0; i < numOfPacket; i++)
	{
		uint32_t start = (i*bytesPerPacket) + numOfHeader;
		uint32_t stop;
		if (i == (numOfPacket - 1))
		{
			stop = sizeBuf - numOfCheckSum;
		}
		else
		{
			stop = (bytesPerPacket * (i + 1)) - numOfCheckSum;
		}
		for (uint32_t k = start; k < stop; k++)
		{
			std::string tempString = util->convertByteToString((*buf).at(k), false);
			boost::to_upper(tempString);
			fileWriteStream << tempString;
			bytesWritten++;
			if (bytesWritten % 16 == 0)
			{
				fileWriteStream << std::endl;
			}
			else
			{
				fileWriteStream << " ";
			}
		}
	}
}

uint32_t ScripterStream::retBytesWritten(void)
{
	return bytesWritten;
}

void ScripterStream::closeWriteFile()
{
	if (txtFile)
	{
		if (bytesWritten % 16 != 0)
		{
			fileWriteStream << "\n";
		}
		fileWriteStream << "q" << std::endl;
	}
	else
	{
		if (bufData.size() != 0)
		{
			bufLine = ":" + util->convertByteToString(bufData.size() / 2, false) + util->fillInZero(util->convertUlongToString(writeAddress, false), 4) + "00" + bufData;
			bufLine += calcAndConvCheckSumHexLine(bufLine);
			fileWriteStream << bufLine << std::endl;
			bufData.clear();
			bufLine.clear();
			writeAddress = 0;
		}
		fileWriteStream << ":00000001FF" << std::endl;
	}
	fileWriteStream.close();
}

void ScripterStream::initLogTxtFile(bool enLogInterpreter, std::string path)
{
	enLog = enLogInterpreter;
	if (enLog)
	{
		std::stringstream streamDate;
		streamDate << date_time.date();
		std::string date = streamDate.str();

		std::stringstream streamTime;
		streamTime << date_time.time_of_day();
		std::string time = streamTime.str();

		std::string logPath;
		if (cliMode)
		{
			logPath = "cli_log";
		}
		else
		{
			logPath = path;
		}

		boost::replace_all(date, "-", "");
		boost::replace_all(time, ":", "");
		boost::replace_all(logPath, ".txt", "");
		logPath += "_" + date + "_" + time + ".log";

		fileLogStream.open(logPath, std::ios::out);
		if (!fileLogStream.good())
		{
			throw std::runtime_error("Failed to create the log file...");
		}
		fileLogStream << showHeaderFile;
	}

}

void ScripterStream::writeLogFile(std::string line, bool tab)
{
	std::string showLine;
	std::string tabShow;
	if (tab)
	{
		tabShow = "\t";
	}
	else
	{
		tabShow = "";
	}

	//Writing log to the logFile
	if (enLog && fileLogStream.good())
	{
		if (util->compareString(line.substr(0, 17), "[SUCCESS_MESSAGE]"))
		{
			fileLogStream << tabShow << line.substr(17, line.size()) << std::endl;
		}
		else
		{
			fileLogStream << tabShow << line << std::endl;
		}

	}

	//Showing text in command prompt / terminal window
	if ((util->compareString(line.substr(0, 15), "[ERROR_MESSAGE]")) ||
		(util->compareString(line.substr(0, 19), "[ACK_ERROR_MESSAGE]")))
	{
		std::cout << red << tabShow << line << std::endl;
		std::cout << white;
	}
	else if ((!enQuiet) && util->compareString(line.substr(0, 17), "[SUCCESS_MESSAGE]"))
	{
		std::cout << green << tabShow << line.substr(17, line.size()) << std::endl;
		std::cout << white;
	}
	else if (!enQuiet)
	{
		std::cout << white << tabShow << line << std::endl;
	}
}

void ScripterStream::writeVerbose(std::vector<uint8_t> *buffer, uint32_t startIdx, uint32_t numOfBytes, uint8_t bracket)
{
	std::string bracket_first;
	std::string bracket_end;
	if (bracket == RECT_BRACKET)
	{
		bracket_first = "[";
		bracket_end = "] ";
	}
	else
	{
		bracket_first = "<";
		bracket_end = "> ";
	}

	if (buffer->size() > 0)
	{
		if (enLog && fileLogStream.good()) fileLogStream << "\t";
		std::cout << "\t";
		uint32_t k = 0;
		for (uint32_t i = startIdx; i < (numOfBytes + startIdx); i++)
		{
			std::string tempStr = util->convertByteToString((*buffer).at(i), false);
			std::string completeStr = bracket_first + tempStr + bracket_end;

			if (enLog && fileLogStream.good()) fileLogStream << completeStr;

			if (!enQuiet)
			{
				std::cout << white << completeStr;
			}

			if ((k + 1) % 16 == 0)
			{
				if (enLog && fileLogStream.good()) fileLogStream << "\n";

				if (!enQuiet)
				{
					std::cout << "\n";
				}

				if (enLog && fileLogStream.good()) fileLogStream << "\t";
				
				if (!enQuiet)
				{
					std::cout << "\t";
				}
			}
			k++;
		}
	}
	if (enLog && fileLogStream.good()) fileLogStream << "\n";
	if (!enQuiet)
	{
		std::cout << "\n";
	}

}

void ScripterStream::closeLogFile()
{
	if ((enLog) && (fileLogStream.good()))
	{
		fileLogStream.close();
	}
}

void ScripterStream::calcTransSpeed(double time, uint32_t numOfbytes, uint8_t mode)
{
	double cntSpeed;
	if (time > 0)
	{
		cntSpeed = (((double)numOfbytes) / 1024.0) / time;
	}
	else
	{
		cntSpeed = 0;
	}

	std::stringstream streamTotalTime;
	streamTotalTime << std::setprecision(4) << time;

	std::stringstream streamCntSpeed;
	streamCntSpeed << std::setprecision(4) << cntSpeed;

	std::string textRead1 = "[SUCCESS_MESSAGE]Time elapsed of writing " + std::to_string(numOfbytes) + " bytes : " + streamTotalTime.str() + " seconds";
	std::string textRead2 = "[SUCCESS_MESSAGE]Speed of writing data :" + streamCntSpeed.str() + "(kB/s)";
	
	std::string textWrite1 = "[SUCCESS_MESSAGE]Time elapsed of reading " + std::to_string(numOfbytes) + " bytes : " + streamTotalTime.str() + " seconds";
	std::string textWrite2 = "[SUCCESS_MESSAGE]Speed of reading data : " + streamCntSpeed.str() + "(kB/s)";

	//write to logfile and show in the screen
	if (mode == READ_MODE)
	{
		writeLogFile(textRead1, true);
		writeLogFile(textRead2, true);
	}
	else
	{
		writeLogFile(textWrite1, true);
		writeLogFile(textWrite2, true);
	}
}
   
std::string ScripterStream::getThePathFile(std::string stInput, std::string relativePath)
{
	std::string resultPath = "";

#if defined(_WIN32) || defined(_WIN64)

	const char* sepSymbol = "\\";
	const char* sepSymbol_ = "/";
	bool pos_doubleDots = util->compareString(stInput.substr(1,1),":");

#else

	const char* sepSymbol = "/";
	bool pos_doubleDots = util->compareString(stInput.substr(0,1),"/");
	
#endif
	bool pos_sameFolder = util->compareString(stInput.substr(0, 2), "./");
	bool pos_upFolder = util->compareString(stInput.substr(0, 3), "../");
	if (pos_doubleDots) // ABSOLUTE PATH
	{
		resultPath = stInput;
	}
	else
	{
		boost::char_separator<char> sep("\\,/");
		boost::tokenizer<boost::char_separator<char>> tokensRelative(relativePath, sep);

		uint8_t cntDeepRelativePath = 0;
		std::vector<std::string> tempBufString;

		for (const std::string& t : tokensRelative)
		{
			tempBufString.push_back(t);
		}
		
		cntDeepRelativePath = tempBufString.size();

		if (pos_upFolder)
		{
			boost::tokenizer<boost::char_separator<char>> tokens(stInput, sep);

			uint8_t cntUpFolder = 0;
			std::string strTemp = "";
			for (const std::string& t : tokens)
			{
				if (t == "..")
				{
					cntUpFolder++;
				}
				else
				{
					strTemp = t;
					break;
				}
			}

			for (uint8_t i = 0; i < (cntDeepRelativePath - cntUpFolder - 1); i++)
			{
				resultPath += tempBufString[i] + sepSymbol;
			}
			resultPath += strTemp;
		}
		else if (pos_sameFolder)
		{
			for (uint8_t i = 0; i < (cntDeepRelativePath - 1); i++)
			{
				resultPath += tempBufString[i] + sepSymbol;
			}
			resultPath += stInput.substr(2, stInput.size());
		}
		else // DOWN FOLDER
		{
			for (uint8_t i = 0; i < (cntDeepRelativePath - 1); i++)
			{
				resultPath += tempBufString[i] + sepSymbol;
			}
			resultPath += stInput;
		}
	}
#if defined(_WIN32) || defined(_WIN64)
	return resultPath;
#else
	std::string temp = "/" + resultPath;
	return temp;
#endif
}

void ScripterStream::showVerbose(std::vector<uint8_t> *txBuffer, std::vector<uint8_t> *rxBuffer,
	uint32_t startIdx, uint32_t numOfResponse, bool enVerbose)
{
	if (enVerbose)
	{
		this->writeVerbose(txBuffer, 0, txBuffer->size(), this->RECT_BRACKET);
		if (numOfResponse != 0)
		{
			this->writeVerbose(rxBuffer, startIdx, numOfResponse, this->TRI_BRACKET);
		}
	}
}

void ScripterStream::showVerboseE4xx(std::vector<uint8_t> *txBuffer, std::vector<uint8_t> *rxBuffer, bool enVerbose)
{
	uint16_t numfOfData = txBuffer->size() - 5;
	uint16_t numfOfGetStatus = 3;
	uint16_t numOfAck = 2;
	uint16_t startIdxData = 0;
	uint16_t startIdxGetStatus = startIdxData + numfOfData;
	uint16_t startIdxAck = startIdxGetStatus + numfOfGetStatus;

	uint16_t startIdxRxAck = 0;
	uint16_t startIdxRxGetStatus = numOfAck;

	if (enVerbose)
	{
		this->writeVerbose(txBuffer, startIdxData, numfOfData, this->RECT_BRACKET);
		this->writeVerbose(rxBuffer, startIdxRxAck, numOfAck, this->TRI_BRACKET);

		this->writeVerbose(txBuffer, startIdxGetStatus, numfOfGetStatus, this->RECT_BRACKET);
		this->writeVerbose(rxBuffer, startIdxRxGetStatus, numfOfGetStatus + numOfAck, this->TRI_BRACKET);
		this->writeVerbose(txBuffer, startIdxAck, numOfAck, this->RECT_BRACKET);
	}
}

void ScripterStream::showVerboseE4xxUsbDfu(std::vector<uint8_t> *txBuffer,
										   std::vector<uint8_t> *rxBuffer,
										   uint16_t numOfTxBufferData,
										   uint16_t numOfRxBufferData,
										   bool enVerbose)
{
	if (enVerbose)
	{
		this->writeVerbose(txBuffer, 0, numOfTxBufferData, this->RECT_BRACKET);
		this->writeVerbose(rxBuffer, 0, numOfRxBufferData, this->TRI_BRACKET);
	}
}