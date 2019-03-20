/*
==DataType.h==

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


/***********************************************************************************
* DataBlock is a struct that been used when reading/writing data to the file
***********************************************************************************/
struct DataBlock
{
	std::vector<uint8_t> data;            // vector that contais data bytes
	uint32_t startAddress;	              // address where the data will be written / was read	
};

/***********************************************************************************
* CommandParams is a struct that used to declare the command and its parameters
***********************************************************************************/
struct CommandParams
{
	Command command;					  // command type from enum Command
	std::vector<std::string> params;      // given parameters
};

/***********************************************************************************
* ModeParams is a struct that used to declare the initialization parameters
***********************************************************************************/
struct ModeParams
{
	Family family;                        // family of the target device
	Protocol protocol;                    // communication protocol that is used
	uint32_t baudRate;                    // baudrate value for UART, I2C, and SPI protocol
	std::string commPort;                 // commport number that is used 
	bool crypto;                          // acknowledge if the CryptoBSL is chosen
	bool f543x;                           // 
	bool enParity;						  // enable the praity bit in UART transmission for certain devices
	std::string macAddress;               // MAC address of the MSP432E target bootloader  
	std::string clientIpAddress;	      // Client IP address for MSP432E target bootloader 
	std::string serverIpAddress;          // Server IP address chosen by the user
	bool useMspFet;                       // Using MSP-FET, the special invocation is required 
	                                      // for communicating with MSP432P4xx bootloader
	uint16_t i2cSlaveAddr;                // Custom I2C Slave address
	uint16_t segmentSize;				  // Memory segment size of the chosen family
	uint16_t blockDataSize;				  // Maximum size in bytes for one data packet transmission
	uint16_t headerSize;				  // Header size in bytes for the BSL packet
	uint16_t checksumSize;				  // Checksum size in bytes for BSL packet
	bool disableAutoBaud;
}; 

/***********************************************************************************
* ProgrammedMemory is a struct that used to part of memory that is programmed
*   based on the firmware input file
***********************************************************************************/
struct ProgrammedMemory
{
	std::vector<std::string> address;        // address where the memory is programmed
	std::vector<std::string> sizeProgrammed; // bytes-length of the memory that is programmed											 
	std::vector<std::string> crcCheck;		 // crc calculation of this programmed-section
};


/***********************************************************************************
* BOOTP packet is the struct for the request and reply packet that starts
*    the Ethernet protocol
***********************************************************************************/
struct BOOTP_PACKET
{
	uint8_t messageType;
	uint8_t hardwareType;
	uint8_t hardwareAddress;
	uint8_t hops;
	uint8_t transactionId[BootpProperties::SIZE_TRANSACTION_ID];
	uint8_t secondElapsed[BootpProperties::SIZE_SECONDS_ELAPSED];
	uint8_t bootpFlags[BootpProperties::SIZE_BOOTP_FLAG];
	uint8_t clientIpAddress[BootpProperties::SIZE_IP_ADDRESS];
	uint8_t yourClientIpAddress[BootpProperties::SIZE_IP_ADDRESS];
	uint8_t nextServerIpAddress[BootpProperties::SIZE_IP_ADDRESS];
	uint8_t relayAgentUpAddress[BootpProperties::SIZE_IP_ADDRESS];
	uint8_t macAddress[BootpProperties::SIZE_HW_ADDRESS];
	uint8_t clientHardwarePaddingAddress[BootpProperties::SIZE_MAC_ADDRESS_PADDING];
	uint8_t serverHostName[BootpProperties::SIZE_SERVER_HOST_NAME];
	uint8_t firmwareFileName[BootpProperties::SIZE_BOOTFILE_NAME];
};

/***********************************************************************************
* TFTP packet is the struct for the request packet to starts the firmware update
*    in Ethernet protocol after the BOOTP initialization is succeed
***********************************************************************************/
struct TFTP_REQUEST_PACKET
{
	uint8_t messageType[TftpProperties::SIZE_TFTP_MESSAGE_TYPE];
	uint8_t firmwareFileName[TftpProperties::SIZE_FIRMWARE_DUMMY];
	uint8_t transferType[TftpProperties::SIZE_TRANSFER_TYPE];
};

/***********************************************************************************
* DfuDevice is the struct that contains the pointer to USB DFU device handle and
*    the USB DFU device interface handle
***********************************************************************************/
struct DfuDevice
{
	libusb_device_handle *handle;
	int32_t deviceInterface;
};

/***********************************************************************************
* DfuStatus is the struct that contains the status, polltimeout, state, and string
*    as te return value from the GET_STATUS command in USB DFU protocol
***********************************************************************************/
struct DfuStatus
{
	uint32_t pollTimeout;
	uint8_t status;
	uint8_t state;
	uint8_t string;
};

/***********************************************************************************
* MSP432EUsbDeviceInfo is the device specific info from MSP432E USB DFU protocol
*    that returns the memory information of the device
***********************************************************************************/
struct MSP432EUsbDeviceInfo
{
	uint32_t partInfo;
	uint32_t classInfo;
	uint32_t flashTop;
	uint32_t appStartAddr;
	uint16_t flashBlock;
	uint16_t numFlashBlock;
};
