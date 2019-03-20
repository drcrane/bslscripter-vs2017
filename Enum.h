/*
==Enum.h==

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
* Define the Command and create the enum class automatically
* These are the commands that could be called from Script / CLI
***********************************************************************************/
#define COMMANDS(DO) \
	DO(LOG) \
	DO(MODE) \
	DO(RX_DATA_BLOCK) \
	DO(RX_PASSWORD) \
	DO(ERASE_SEGMENT) \
	DO(TOGGLE_INFO) \
	DO(ERASE_BLOCK) \
	DO(MASS_ERASE) \
	DO(CRC_CHECK) \
	DO(SET_PC) \
	DO(TX_DATA_BLOCK) \
	DO(TX_BSL_VERSION) \
	DO(TX_BUFFER_SIZE) \
	DO(RX_DATA_BLOCK_FAST) \
	DO(RX_DATA_BLOCK_32) \
	DO(RX_PASSWORD_32) \
	DO(ERASE_SEGMENT_32) \
	DO(REBOOT_RESET) \
	DO(CRC_CHECK_32) \
	DO(SET_PC_32) \
	DO(TX_DATA_BLOCK_32) \
	DO(TX_BSL_VERSION_32) \
	DO(RX_SECURE_DATA_BLOCK) \
	DO(CHANGE_BAUD_RATE) \
	DO(FACTORY_RESET) \
	DO(JTAG_LOCK) \
	DO(JTAG_PASSWORD) \
	DO(DOWNLOAD) \
	DO(GET_STATUS) \
	DO(ACK_HOST) \
	DO(VERBOSE)	\
	DO(DELAY) \
	DO(CUSTOM_COMMAND) \
	DO(DISABLE_BIN_FILE) \
	DO(APP_VERSION)

#define MAKE_ENUM_COMMANDS(VAR) VAR,	
enum class Command
{
	COMMANDS(MAKE_ENUM_COMMANDS)
};


/***********************************************************************************
* Define the Family and create the enum class automatically
* Later in the initialization, the families will be classified/generalized into:
* - P4xx
* - F543x
* - Flash
* - FRAM
* - Crypto
***********************************************************************************/
#define FAMILY(DO) \
	DO(P4xx) \
	DO(MSP432P4xx) \
	DO(MSP430F543x) \
	DO(F543x) \
	DO(F543xfamily) \
	DO(MSP430F5xx) \
	DO(F5xx) \
	DO(F6xx) \
	DO(MSP430F6xx) \
	DO(Flash) \
	DO(Fram) \
	DO(FRxx) \
	DO(MSP430FRxx) \
	DO(E4xx) \
	DO(MSP432E4xx) \
	DO(Crypto)

#define MAKE_ENUM_FAMILY(VAR) VAR,	
enum class Family
{
	FAMILY(MAKE_ENUM_FAMILY)
};

/***********************************************************************************
* Define the Protocol and create the enum class automatically
***********************************************************************************/
#define PROTOCOL(DO) \
	DO(Uart) \
	DO(I2c) \
	DO(Spi) \
	DO(Usb) \
	DO(Ethernet)

#define MAKE_ENUM_PROTOCOL(VAR) VAR,	
enum class Protocol
{
	PROTOCOL(MAKE_ENUM_PROTOCOL)
};

/***********************************************************************************
* Define the Reading File status
***********************************************************************************/
enum class ReadLineStatus
{
	AddressFound,	 //status when a new address is found
	ExtAddressFound, //applicable for Intel Hex File format when the extended address is found
	NormalRead,		 //status when reading the data bytes
	QuitSign		 //status when the end of file is found
};

/***********************************************************************************
* Define the Reading File process
***********************************************************************************/
enum class ScripterReadStatus
{
	OnGoing,        //process reading the data bytes
	DataBlockReady  //packet of data is ready to be transmitted
};

/***********************************************************************************
* Define memory size of families
***********************************************************************************/
namespace ProgramMemorySize
{
	const uint32_t MSP432E = 0x100000; // 1024kB flash memory
}

/***********************************************************************************
* Define segment size of families
***********************************************************************************/
namespace SegmentSize
{
	const uint16_t MSP430 = 512;
	const uint16_t MSP432P = 4096;
	const uint16_t MSP432E = 16384;
}

/***********************************************************************************
* Define default password size of families
***********************************************************************************/
namespace PasswordSize
{
	const uint16_t PWD_16_BIT = 16;
	const uint16_t PWD_32_BIT = 32;
	const uint16_t PWD_256_BIT = 256;
}

/***********************************************************************************
* Define block data size of families
***********************************************************************************/
namespace BlockDataSize
{
	const uint16_t MSP430_MSP432P_SERIAL = 256;
	const uint16_t MSP430_USB = 32;
	const uint16_t MSP432E_SERIAL = 60;
	const uint16_t MSP432E_ETHERNET = 512;
	const uint16_t MSP432E_USBDFU = 1024;
};	

/***********************************************************************************
* Define header size of families
***********************************************************************************/
namespace HeaderSize
{
	const uint16_t MSP432E = 2;
	const uint16_t MSP430_USB = 3;
	const uint16_t MSP430_MSP432P = 4;
	const uint16_t MSP432E_USBDFU = 0;
};

/***********************************************************************************
* Define checksum size of families
***********************************************************************************/
namespace CheckSumSize
{
	const uint16_t MSP432E = 1;
	const uint16_t MSP430_USB = 0;
	const uint16_t MSP430_MSP432P = 2;
	const uint16_t MSP432E_USBDFU = 0;
};

/***********************************************************************************
* Define properties of Wake-on-LAN packet
***********************************************************************************/
namespace WakeOnLan
{
	const uint8_t WOL_MAGIC_PACKET_MARKER = 0xAA;
	const uint8_t MAC_REP_WOL_MAGIC_PACKET = 0x04;
}
/***********************************************************************************
* Define properties of BOOTP packet
***********************************************************************************/
namespace BootpProperties
{
	const uint8_t SIZE_HOSTNAME_1 = 4;
	const uint8_t SIZE_HOSTNAME_2 = 8;
	const uint8_t SIZE_SERVER_HOST_NAME = 64;
	const uint8_t SIZE_FIRMWARE_DUMMY = 12;
	const uint8_t SIZE_BOOTFILE_NAME = 128;
	const uint8_t SIZE_VENDOR_NAME = 64;
	const uint8_t SIZE_HW_ADDRESS = 0x06;
	const uint8_t SIZE_TRANSACTION_ID = 4;
	const uint8_t SIZE_SECONDS_ELAPSED = 2;
	const uint8_t SIZE_BOOTP_FLAG = 2;
	const uint8_t SIZE_IP_ADDRESS = 4;
	const uint8_t SIZE_MAC_ADDRESS_PADDING = 10;

	const uint8_t BOOT_REQUEST = 0x01;
	const uint8_t BOOT_REPLY = 0x02;
	const uint8_t HARDWARE_TYPE_ETHERNET = 0x01;
	const uint8_t HOPS = 0x00;
	const uint8_t hostName1[SIZE_HOSTNAME_1] = { 0x74, 0x69, 0x76, 0x61 };
	const uint8_t hostName2[SIZE_HOSTNAME_2] = { 0x6D, 0x73, 0x70, 0x34, 0x33, 0x32, 0x65, 0x34 };
	const uint8_t firmwareDummy[SIZE_FIRMWARE_DUMMY] = { 'f', 'i', 'r', 'm', 'w', 'a', 'r', 'e', '.', 'b', 'i', 'n' };
	const uint8_t DEFAULT_BYTE = 0x00;
}

/***********************************************************************************
* Define properties of TFTP packet
***********************************************************************************/
namespace TftpProperties
{
	const uint8_t SIZE_TFTP_MESSAGE_TYPE = 2;
	const uint8_t SIZE_FIRMWARE_DUMMY = 12;
	const uint8_t SIZE_TRANSFER_TYPE = 7;

	const uint16_t TFTP_RRQ = 0x0001;
	const uint16_t TFTP_WRQ = 0x0002;
	const uint16_t TFTP_DATA = 0x0003;
	const uint8_t  octetTransferType[SIZE_TRANSFER_TYPE] = { 0x00, 'o', 'c', 't', 'e', 't', 0x00 };
}

/***********************************************************************************
* Define properties of USB DFU
***********************************************************************************/
namespace Msp432eUsbDfuProperties
{
	const uint16_t VID = 0x1CBE;
	const uint16_t PID = 0x00FF;
	const uint32_t MAX_TRANSFER_SIZE = 0x400;
	const uint8_t REQUEST_TYPE = 0xA1;
	const uint8_t REQUEST_MSP432E = 0x42;
	const uint8_t REQUEST_MSP432_VALUE = 0x23;
	const uint16_t MSP432E_MARKER = 0x4C4D;
	const uint16_t MSP432E_VERSION = 0x0001;
	const uint32_t DETACH_TIMEOUT = 1000; // in miliseconds
	const uint32_t FLASH_TIMEOUT = 20000; // in miliseconds 
	const uint8_t COMMAND_SIZE = 8; // command in bytes
}

namespace UsbDfuStandardProperties
{
	const uint8_t CLASS_APP = 0xFE; // Application Specific Class code
	const uint8_t SUB_CLASS = 0x01; // Device firmware upgrade code							  
	const uint8_t INTERFACE_DFU_PROTOCOL = 0x02; // DFU mode protocol
	const uint8_t FUNC_DESCRIPTOR_TYPE = 0x21; // Descriptor type defined for DFU Type
	const uint8_t REQUEST_TRANSFER_IN = (LIBUSB_ENDPOINT_IN |
		                                 LIBUSB_REQUEST_TYPE_CLASS |
		                                 LIBUSB_RECIPIENT_INTERFACE);
}

namespace UsbDfuStandardCommand
{
	const uint8_t DETACH = 0x00;
	const uint8_t DNLOAD = 0x01;
	const uint8_t UPLOAD = 0x02;
	const uint8_t GETSTATUS = 0x03;
	const uint8_t CLRSTATUS = 0x04;
	const uint8_t GETSTATE = 0x05;
	const uint8_t ABORT = 0x06;
}

namespace Msp432ECommandSize
{
	const uint8_t SIZE = 8;	//Msp432ESpecificCommand packet always have 8 bytes length
	const uint8_t SIZE_INFO = 1;
}

namespace Msp432ESpecificCommand
{
	const uint8_t PROG = 0x01;	
	const uint8_t READ = 0x02;	
	const uint8_t CHECK = 0x03;
	const uint8_t ERASE = 0x04;	
	const uint8_t INFO = 0x05; 	
	const uint8_t BIN = 0x06;	
	const uint8_t RESET = 0x07;	
	const uint8_t BIN_ENABLE = 0x00;	
	const uint8_t BIN_DISABLE = 0x01; 

	const uint8_t RESERVED_VALUE = 0x00;
}

namespace UsbDfuStandardStatus
{
	const uint8_t OK = 0x00;
	const uint8_t ERROR_TARGET = 0x01;
	const uint8_t ERROR_FILE = 0x02;
	const uint8_t ERROR_WRITE = 0x03;
	const uint8_t ERROR_ERASE = 0x04;
	const uint8_t ERROR_CHECK_ERASED = 0x05;
	const uint8_t ERROR_PROG = 0x06;
	const uint8_t ERROR_VERIFY = 0x07;
	const uint8_t ERROR_ADDRESS = 0x08;
	const uint8_t ERROR_NOTDONE = 0x09;
	const uint8_t ERROR_FIRMWARE = 0x0A;
	const uint8_t ERROR_VENDOR = 0x0B;
	const uint8_t ERROR_USBR = 0x0C;
	const uint8_t ERROR_POR = 0x0D;
	const uint8_t ERROR_UNKNOWN = 0x0E;
	const uint8_t ERROR_STALLEDPKT = 0x0F;
}

namespace UsbDfuStandardState
{
	const uint8_t APP_IDLE = 0x00;
	const uint8_t APP_DETACH = 0x01;
	const uint8_t DFU_IDLE = 0x02;
	const uint8_t DFU_DOWNLOAD_SYNC = 0x03;
	const uint8_t DFU_DOWNLOAD_BUSY = 0x04;
	const uint8_t DFU_DOWNLOAD_IDLE = 0x05;
	const uint8_t DFU_MANIFEST_SYNC = 0x06;
	const uint8_t DFU_MANIFEST = 0x07;
	const uint8_t DFU_MANIFEST_WAIT_RESET = 0x08;
	const uint8_t DFU_UPLOAD_IDLE = 0x09;
	const uint8_t DFU_ERROR = 0x0A;
}

namespace UsbDfuResponseSize
{
	const uint32_t GET_STATUS = 6;
	const uint32_t MSP432E_SPEC_COMMAND_CLASS = 4;
	const uint32_t MSP432E_DEVICE_INFO = 20;
}