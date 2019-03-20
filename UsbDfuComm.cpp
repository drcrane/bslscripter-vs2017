/*
== UsbDfuComm.cpp ==

Copyright(c) 2015 - 2018 Texas Instruments Incorporated

All rights reserved not granted herein.

Limited License.

Texas Instruments Incorporated grants a world - wide, royalty - free,
non - exclusive license under copyrights and patents it now or
hereafter owns or controls to make, have made, use, import,
offer to sell and sell("Utilize") this software subject to
the terms herein.With respect to the foregoing patent license,
such license is granted  solely to the extent that any such patent
is necessary to Utilize the software alone.The patent license
shall not apply to any combinations which include this software,
other than combinations with devices manufactured by or for TI
(“TI Devices”).No hardware patent is licensed hereunder.

Redistributions must preserve existing copyright notices and
reproduce this license(including the above copyright notice
and the disclaimer and(if applicable) source code license
limitations below) in the documentation and / or other materials
provided with the distribution

Redistribution and use in binary form, without modification,
are permitted provided that the following conditions are met :

*No reverse engineering, decompilation, or disassembly of
this software is permitted with respect to any software
provided in binary form.

*	any redistribution and use are licensed by TI for use only
with TI Devices.

*	Nothing shall obligate TI to provide you with source code
for the software licensed and provided to you in object code.

If software source code is provided to you, modification and
redistribution of the source code are permitted provided that
the following conditions are met :

*any redistribution and use of the source code, including any
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
PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL TI AND TI’S LICENSORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "stdafx.h"

UsbDfuComm::UsbDfuComm(ModeParams* modeParams, Util *util)
	:modeParams(modeParams), 
	 util(util),
	 rxBuffer(Msp432eUsbDfuProperties::MAX_TRANSFER_SIZE),
	 blockIndex(0),
	 bytesWritten(0),
	 bytesRead(0),
	 dfuDevice(new DfuDevice()),
	 dfuStatus(new DfuStatus()),
	 msp432eDeviceInfo(new MSP432EUsbDeviceInfo()),
	 busNumber(0),
	 deviceAddress(0),
	 startRead(false)
{
	if (dfuStatus == nullptr)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Initialize the DFU status pointer failed!");
	}
	if (dfuDevice == nullptr)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Initialize the DFU device pointer failed!");
	}
	if (msp432eDeviceInfo == nullptr)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Initialize the DFU Device Info pointer failed!");
	}
}

UsbDfuComm::~UsbDfuComm()
{
	if (dfuStatus != nullptr)
	{
		delete dfuStatus;
		dfuStatus = nullptr;
	}

	if (dfuDevice != nullptr)
	{
		delete dfuDevice;
		dfuDevice = nullptr;
	}

	if (msp432eDeviceInfo != nullptr)
	{
		delete msp432eDeviceInfo;
		msp432eDeviceInfo = nullptr;
	}
}

void UsbDfuComm::init(ModeParams* modeParams)
{
	// Initiliaze LIBUSB
	// Successful initialization return LIBUSB_SUCCESS which has return value 0
	int32_t result = libusb_init(&this->context);
	if (result != LIBUSB_SUCCESS)
	{
		throw std::runtime_error(translateLibUsbErrorCode(result));
	}

	try
	{
		// Open device handle of USB DFU device
		if (dfuDeviceInit() != nullptr)
		{
			// Make sure the device is in DFU Idle State
			dfuMakeIdle(true);

			// Send MSP432E Specific Command , to validate the USB DFU version on the device
			dfuMsp432eSpecificCommandClass();

			// Obtain the device info 
			dfuGetDeviceInfo();

			// Bring back device to Idle State
			dfuGetStatus();
		}
		else
		{
			throw std::runtime_error("[ERROR_MESSAGE]Initialization of USB DFU failed!");
		}
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
	}
 }

void UsbDfuComm::transmitBuffer(std::vector<uint8_t>* txBuffer)
{
	std::vector<uint8_t> checkRebootCommand(8, Msp432ESpecificCommand::RESERVED_VALUE);
	checkRebootCommand.at(0) = Msp432ESpecificCommand::RESET;

	if ((txBuffer->size() == 8) && 
		(std::equal(checkRebootCommand.begin(), checkRebootCommand.end(), txBuffer->begin())))
	{
		dfuTransferOutResetDevice(UsbDfuStandardCommand::DNLOAD,
			                      blockIndex,
			                      &txBuffer->at(0),
			                      txBuffer->size());
	}
	else
	{
		dfuTransferOut(UsbDfuStandardCommand::DNLOAD,
			           blockIndex,
			           &txBuffer->at(0),
			           txBuffer->size());

		dfuGetStatus();

		while (dfuStatus->state != UsbDfuStandardState::DFU_IDLE ||
			dfuStatus->state != UsbDfuStandardState::DFU_DOWNLOAD_IDLE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(dfuStatus->pollTimeout));
			dfuGetStatus();

			//When the memory programming is on-going, the USB DFU will return the status
			//   of DFU_DOWNLOAD_IDLE. To continue for the next block, the blockIndex
			//   shall be increased by one, and also the total bytesWritten to the device
			//   shall be calculated. 

			if (dfuStatus->state == UsbDfuStandardState::DFU_DOWNLOAD_IDLE)
			{
				blockIndex++;
				bytesWritten += txBuffer->size();
				break;
			}

			// When the host send other command that memory programming, USB DFU will return
			//    the status of DFU_IDLE. There is not needed to increased the blockIndex.
			//    the blockIndex shall stays 0. Counting the number of bytesWritten is also
			//    not needed.

			if (dfuStatus->state == UsbDfuStandardState::DFU_IDLE)
			{
				break;
			}
		}
	}
}

std::vector<uint8_t>* UsbDfuComm::receiveBuffer(int32_t size)
{
	// Waiting the USB DFU target to process the data that has been
	//    transmitted and go to the DFU_DOWNLOAD_IDLE or DFU_IDLE mode.
	while (dfuStatus->state != UsbDfuStandardState::DFU_DOWNLOAD_IDLE ||
		   dfuStatus->state != UsbDfuStandardState::DFU_IDLE)
	{
		dfuGetStatus();
		std::this_thread::sleep_for(std::chrono::milliseconds(dfuStatus->pollTimeout));

		// If the device enter the DFU_DOWNLOAD_IDLE, that means it finished
		//   programming the memory with transmitted data. Next step is to 
		//   send the Payload 0 packet to tell the device that the next packet will
		//   be sent.

		if (dfuStatus->state == UsbDfuStandardState::DFU_DOWNLOAD_IDLE)
		{
			// Send the payload 0
			dfuTransferOut(UsbDfuStandardCommand::DNLOAD,
				           0,
				           NULL,
				           0);
			// Get status from the last transmission Payload 0 packet
			//    this command will change the state of device into DFU_IDLE.
			//    If the device is not entering the DFU_IDLE, throw the exception.
			dfuGetStatus();
			if (dfuStatus->state != UsbDfuStandardState::DFU_IDLE)
			{
				throw std::runtime_error(translateDfuStateToString(dfuStatus->state));
			}
			break;
		}
		else if (dfuStatus->state == UsbDfuStandardState::DFU_IDLE)
		{
			if (startRead)
			{
				dfuTransferIn(UsbDfuStandardProperties::REQUEST_TRANSFER_IN,
					          UsbDfuStandardCommand::UPLOAD,
					          blockIndex,
					          (uint8_t*)&rxBuffer.at(0),
					          size);
				blockIndex++;
				bytesRead += size;
			}
			break;
		}
		else if (dfuStatus->state == UsbDfuStandardState::DFU_UPLOAD_IDLE)
		{
			dfuTransferIn(UsbDfuStandardProperties::REQUEST_TRANSFER_IN,
				          UsbDfuStandardCommand::UPLOAD,
						  blockIndex,
						  (uint8_t*)&rxBuffer.at(0),
						  size);
			blockIndex++;
			bytesRead += size;
			startRead = false;
			break;
		}
	}
	return &rxBuffer;
}

void UsbDfuComm::changeBaudRate(uint32_t baudRate)
{
	// This has not been implemented yet
}

void UsbDfuComm::close()
{
	libusb_close(this->dfuDevice->handle);
	libusb_exit(this->context);
}

void UsbDfuComm::resetBytesWritten()
{
	blockIndex = 0;	   // blockIndex is wValue in libusb_control_transfer
	bytesWritten = 0;  // total data from firmware image that has been programmed into target device
}

uint32_t UsbDfuComm::retBytesWritten()
{
	// Number of actual bytes programmed is counted from:
	// ((Total block + 1 ) / 2 )* command size
	// From all total blocks (started from index 0),
	//    packet always sent with the command packet, follow by the data packet	.
	// We have to substract the command packet, as it is not the actual
	//    data being programmed in the target device
	return (bytesWritten - (((Msp432ECommandSize::SIZE * blockIndex) + 1) / 2));
}

void UsbDfuComm::resetBytesSent()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the packet is sent as a whole.
	// In other family and protocol which using this, the block of data
	//    is sent in several small packets.
}

uint32_t UsbDfuComm::retBytesSent()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the packet is sent as a whole.
	// In other family and protocol which using this, the block of data
	//    is sent in several small packets.
	return 0;
}

uint32_t UsbDfuComm::retBytesRead()
{
	return bytesRead;
}

uint8_t UsbDfuComm::retAck()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
	return 0;
}

uint32_t UsbDfuComm::retLastRxBufferIdx()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the packet is sent as a whole.
	// In other family and protocol which using this, the block of data
	//    is sent in several small packets.
	return 0;
}

std::vector<uint8_t>* UsbDfuComm::retPtrRxBuffer()
{
	return &rxBuffer;
}

std::vector<uint8_t>* UsbDfuComm::retPtrTxBuffer()
{
	return &txBuffer;
}

void UsbDfuComm::resetIdxAcc()
{
	bytesRead = 0; 
	startRead = true;
}

void UsbDfuComm::processResponse(uint8_t ack, std::vector<uint8_t> *response)
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
}

bool UsbDfuComm::retErrorAck()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
	return false;
}

bool UsbDfuComm::retErrorStatus()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
	return false;
}

std::string UsbDfuComm::retErrorAckMessage()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
	return "";
}

std::string UsbDfuComm::retErrorStatusMessage()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
	return "";
}

bool UsbDfuComm::retCrcCheckResult()
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
	return false;
}

std::string UsbDfuComm::translateResponse(CommandParams *cmdParams, std::vector<uint8_t>* buffer, uint8_t idx)
{
	// This is not implemented for the MSP432E USB DFU, 
	//    as the response is handled by the dfuTransferIn and dfuTranferOut
	//    following the standard of UsbDfu
	return "";
}

std::string UsbDfuComm::translateLibUsbErrorCode(int32_t errorCode)
{
	static std::map<int32_t, std::string> LibUsbErrorStatus =
	{
		{ LIBUSB_SUCCESS, "[SUCCESSFUL_MESSAGE]Successful execution!" },
		{ LIBUSB_ERROR_IO, "[ERROR_MESSAGE]LIBUSB_ERROR_IO"},
		{ LIBUSB_ERROR_INVALID_PARAM, "[ERROR_MESSAGE]LIBUSB_ERROR_INVALID_PARAM"},
		{ LIBUSB_ERROR_ACCESS, "[ERROR_MESSAGE]LIBUSB_ERROR_ACCESS"},
		{ LIBUSB_ERROR_NO_DEVICE, "[ERROR_MESSAGE]LIBUSB_ERROR_NO_DEVICE"},
		{ LIBUSB_ERROR_NOT_FOUND, "[ERROR_MESSAGE]LIBUSB_ERROR_NOT_FOUND"},
		{ LIBUSB_ERROR_BUSY, "[ERROR_MESSAGE]LIBUSB_ERROR_BUSY"},
		{ LIBUSB_ERROR_TIMEOUT, "[ERROR_MESSAGE]LIBUSB_ERROR_TIMEOUT" },
		{ LIBUSB_ERROR_OVERFLOW, "[ERROR_MESSAGE]LIBUSB_ERROR_OVERFLOW" },
		{ LIBUSB_ERROR_PIPE, "[ERROR_MESSAGE]LIBUSB_ERROR_PIPE" },
		{ LIBUSB_ERROR_INTERRUPTED, "[ERROR_MESSAGE]LIBUSB_ERROR_INTERRUPTED" },
		{ LIBUSB_ERROR_NO_MEM, "[ERROR_MESSAGE]LIBUSB_ERROR_NO_MEM" },
		{ LIBUSB_ERROR_NOT_SUPPORTED, "[ERROR_MESSAGE]LIBUSB_ERROR_NOT_SUPPORTED" },
		{ LIBUSB_ERROR_OTHER, "[ERROR_MESSAGE]LIBUSB_ERROR_OTHER" },
	};

	auto isFound = LibUsbErrorStatus.find(errorCode);

	if (isFound != LibUsbErrorStatus.end())
	{
		return  LibUsbErrorStatus[errorCode];
	}
	else
	{
		return "[ERROR_MESSAGE]Unknown LIBUSB_ERROR";
	}
}

std::string UsbDfuComm::translateDfuStatusToString(const uint8_t status)
{
	static std::map<uint8_t, std::string> UsbDfuStatus =
	{
		{ UsbDfuStandardStatus::OK, "[SUCCESSFUL_MESSAGE]USB DFU Status OK!" },
		{ UsbDfuStandardStatus::ERROR_TARGET, "[ERROR_MESSAGE]USB DFU Error Target!" },
		{ UsbDfuStandardStatus::ERROR_FILE, "[ERROR_MESSAGE]USB DFU Error File!" },
		{ UsbDfuStandardStatus::ERROR_WRITE, "[ERROR_MESSAGE]USB DFU Error Write!" },
		{ UsbDfuStandardStatus::ERROR_ERASE, "[ERROR_MESSAGE]USB DFU Error Erase!" },
		{ UsbDfuStandardStatus::ERROR_CHECK_ERASED, "[ERROR_MESSAGE]USB DFU Error Checked Erased!" },
		{ UsbDfuStandardStatus::ERROR_PROG, "[ERROR_MESSAGE]USB DFU Error Programming!" },
		{ UsbDfuStandardStatus::ERROR_VERIFY, "[ERROR_MESSAGE]USB DFU Error Verifying!" },
		{ UsbDfuStandardStatus::ERROR_ADDRESS, "[ERROR_MESSAGE]USB DFU Error Address!" },
		{ UsbDfuStandardStatus::ERROR_NOTDONE, "[ERROR_MESSAGE]USB DFU Error Not Done!" },
		{ UsbDfuStandardStatus::ERROR_FIRMWARE, "[ERROR_MESSAGE]USB DFU Error Firmware!" },
		{ UsbDfuStandardStatus::ERROR_VENDOR, "[ERROR_MESSAGE]USB DFU Error Vendor!" },
		{ UsbDfuStandardStatus::ERROR_USBR, "[ERROR_MESSAGE]USB DFU Error USBR!" },
		{ UsbDfuStandardStatus::ERROR_POR, "[ERROR_MESSAGE]USB DFU Error POR!" },
		{ UsbDfuStandardStatus::ERROR_UNKNOWN, "[ERROR_MESSAGE]USB DFU Error Unknown!" },
		{ UsbDfuStandardStatus::ERROR_STALLEDPKT, "[ERROR_MESSAGE]USB DFU Error Stalled!" },
	};

	auto isFound = UsbDfuStatus.find(status);

	if (isFound != UsbDfuStatus.end())
	{
		return  UsbDfuStatus[status];
	}
	else
	{
		return "[ERROR_MESSAGE]Undefined USB DFU Error Code!";
	}
}

std::string UsbDfuComm::translateDfuStateToString(const uint8_t state)
{
	static std::map<uint8_t, std::string> UsbDfuState =
	{
		{ UsbDfuStandardState::APP_IDLE, "[SUCCESSFUL_MESSAGE]USB APP Idle State!" },
		{ UsbDfuStandardState::APP_DETACH, "[SUCCESSFUL_MESSAGE]USB APP Detach State!"},
		{ UsbDfuStandardState::DFU_IDLE, "[SUCCESSFUL_MESSAGE]USB DFU Idle State!"},
		{ UsbDfuStandardState::DFU_DOWNLOAD_SYNC, "[SUCCESSFUL_MESSAGE]USB DFU Download Sync State!"},
		{ UsbDfuStandardState::DFU_DOWNLOAD_BUSY, "[SUCCESSFUL_MESSAGE]USB DFU Download Busy State!"},
		{ UsbDfuStandardState::DFU_DOWNLOAD_IDLE, "[SUCCESSFUL_MESSAGE]USB DFU Download Idle State!"},
		{ UsbDfuStandardState::DFU_MANIFEST_SYNC, "[SUCCESSFUL_MESSAGE]USB DFU Manifest Sync State!"},
		{ UsbDfuStandardState::DFU_MANIFEST, "[SUCCESSFUL_MESSAGE]USB DFU Manifest State!"},
		{ UsbDfuStandardState::DFU_MANIFEST_WAIT_RESET, "[SUCCESSFUL_MESSAGE]USB DFU Manifest Wait Reset State!" },
		{ UsbDfuStandardState::DFU_UPLOAD_IDLE, "[SUCCESSFUL_MESSAGE]USB DFU Upload Idle State!" },
		{ UsbDfuStandardState::DFU_ERROR, "[ERROR_MESSAGE]USB DFU Error State!"}
  	};

	auto isFound = UsbDfuState.find(state);

	if (isFound != UsbDfuState.end())
	{
		return  UsbDfuState[state];
	}
	else
	{
		return "[ERROR_MESSAGE]Undefined USB DFU State!";
	}
}

libusb_device* UsbDfuComm::dfuDeviceInit()
{
	libusb_device **list;
 	size_t deviceCount = libusb_get_device_list(context, &list);
	int32_t result;

	// Get descriptor from each devices attached to the host
	// to find the MSP432E USB DFU target device
	for (size_t i = 0; i < deviceCount; i++)
	{
		libusb_device *device = list[i];
		libusb_device_descriptor descriptor;

		result = libusb_get_device_descriptor(device, &descriptor);
		if (result != LIBUSB_SUCCESS)
		{
			throw std::runtime_error(translateLibUsbErrorCode(result));
		}

		// Check the VID and PID if it is MSP432E device
		if ((descriptor.idVendor == Msp432eUsbDfuProperties::VID) &&
			(descriptor.idProduct == Msp432eUsbDfuProperties::PID))
		{
			this->busNumber = libusb_get_bus_number(device);
			this->deviceAddress = libusb_get_device_address(device);

			std::cout << "\tFound device at USB port with device bus number : "
				     << util->convertByteToString(libusb_get_bus_number(device), true) << std::endl
				     << "\tand device address is: "
				     << util->convertByteToString(libusb_get_device_address(device), true) << std::endl;

			// Check if the MSP432E device has the Class and SubClass of DFU 
			int32_t interfaceNum = dfuFindInterface(device, descriptor.bNumConfigurations);
			dfuDevice->deviceInterface = interfaceNum;
			result = libusb_open(device, &dfuDevice->handle);
			if (result == LIBUSB_SUCCESS)
			{
				result = libusb_set_configuration(dfuDevice->handle, 1);
				if (result == LIBUSB_SUCCESS)
				{
					//Successfuly initialize USB DFU device, return its device handle
					return device;
				}
				else
				{
					throw std::runtime_error(translateLibUsbErrorCode(result));
				}
			}
			else
			{
				throw std::runtime_error(translateLibUsbErrorCode(result));
			}
		}
	}
	libusb_free_device_list(list, 1);
	return nullptr;
}

int32_t UsbDfuComm::dfuFindInterface(libusb_device *device, uint8_t numConfigurations)
{
	// Loop through all descriptor config from USBdevice handle
	for (uint8_t c = 0; c < numConfigurations; c++)
	{
		libusb_config_descriptor *config;
		int32_t result = libusb_get_config_descriptor(device, c, &config);

		if (result != LIBUSB_SUCCESS)
		{
			throw std::runtime_error(translateLibUsbErrorCode(result));
		}

		// From device config, get the interface list, then
		// search for the USB DFU scpecific interface class and subclass
		for (uint8_t i = 0; i < config->bNumInterfaces; i++)
		{
			libusb_interface interfaceList = config->interface[i];

			// Loop through all settings
			for (int32_t s = 0; s < interfaceList.num_altsetting; s++)
			{
				libusb_interface_descriptor setting;
				setting = interfaceList.altsetting[s];

				// Check if the interface is the USB DFU interface
				if ((setting.bInterfaceClass == UsbDfuStandardProperties::CLASS_APP) &&
					(setting.bInterfaceSubClass == UsbDfuStandardProperties::SUB_CLASS))
				{
					std::cout << "\tFound DFU interface : " << util->convertByteToString(setting.bInterfaceNumber, true) << std::endl;
					return setting.bInterfaceNumber;
				}
			}
			libusb_free_config_descriptor(config);
		}
	}

	// Case where USB DFU is not found
	throw std::runtime_error("[ERROR_MESSAGE]No USB DFU interface class is found!");
}

void UsbDfuComm::dfuTransferOut(uint8_t request,
	                            const int32_t value,
								uint8_t* data,
								uint16_t length)
{
	int32_t result = libusb_control_transfer(dfuDevice->handle,
		             (LIBUSB_ENDPOINT_OUT |
		              LIBUSB_REQUEST_TYPE_CLASS |
		              LIBUSB_RECIPIENT_INTERFACE),
		              request,
		              value,
		              dfuDevice->deviceInterface,
		              data,
		              length,
		              Msp432eUsbDfuProperties::FLASH_TIMEOUT);
	if (length == 8 &&
		*data++ == Msp432ESpecificCommand::RESET &&
		*data++ == Msp432ESpecificCommand::RESERVED_VALUE  &&
		*data++ == Msp432ESpecificCommand::RESERVED_VALUE  &&
		*data++ == Msp432ESpecificCommand::RESERVED_VALUE  &&
		*data++ == Msp432ESpecificCommand::RESERVED_VALUE  &&
		*data++ == Msp432ESpecificCommand::RESERVED_VALUE  &&
		*data++ == Msp432ESpecificCommand::RESERVED_VALUE &&
		*data == Msp432ESpecificCommand::RESERVED_VALUE)
	{
		uint8_t stop = 0;
	}
	else
	{
		if (result != length)
		{
			throw std::runtime_error(translateLibUsbErrorCode(result));
		}
	}
}

void UsbDfuComm::dfuTransferIn(uint8_t requestType,
	                           uint8_t request,
	                           const int32_t value,
							   uint8_t* data,
							   uint16_t length)
{
	int32_t result = libusb_control_transfer(dfuDevice->handle,
		                                     (LIBUSB_ENDPOINT_IN |
		                                      LIBUSB_REQUEST_TYPE_CLASS |
											  LIBUSB_RECIPIENT_INTERFACE),
											 request,
											 value,
											 dfuDevice->deviceInterface,
											 data,
											 length,
											 Msp432eUsbDfuProperties::FLASH_TIMEOUT);

	if (result != length)
	{
		throw std::runtime_error(translateLibUsbErrorCode(result));
	}
}


void UsbDfuComm::dfuTransferOutResetDevice(uint8_t request,
	                                       const int32_t value,
							               uint8_t* data,
										   uint16_t length)
{
	int32_t result = libusb_control_transfer(dfuDevice->handle,
		(LIBUSB_ENDPOINT_OUT |
		LIBUSB_REQUEST_TYPE_CLASS |
		LIBUSB_RECIPIENT_INTERFACE),
		request,
		value,
		dfuDevice->deviceInterface,
		data,
		length,
		Msp432eUsbDfuProperties::FLASH_TIMEOUT);
}

void UsbDfuComm::dfuAbort()
{
	dfuTransferOut(UsbDfuStandardCommand::ABORT,
				   0,
				   NULL,
				   0);
}

void UsbDfuComm::dfuMakeIdle(bool initialAbort)
{
	if (initialAbort)
	{
		dfuAbort();
	}

	uint8_t retries = 4;

	while (retries > 0)
	{
		retries--;

		dfuGetStatus();
		switch (dfuStatus->state)
		{
		case UsbDfuStandardState::DFU_IDLE:
			retries = 0;
			break;

		case UsbDfuStandardState::DFU_DOWNLOAD_SYNC:
		case UsbDfuStandardState::DFU_DOWNLOAD_IDLE:
		case UsbDfuStandardState::DFU_MANIFEST_SYNC:
		case UsbDfuStandardState::DFU_UPLOAD_IDLE:
		case UsbDfuStandardState::DFU_DOWNLOAD_BUSY:
		case UsbDfuStandardState::DFU_MANIFEST:
			dfuAbort();
			break;

		case UsbDfuStandardState::DFU_ERROR:
			dfuClearStatus();
			break;

		case UsbDfuStandardState::APP_IDLE:
			dfuDetach();
			break;

		case UsbDfuStandardState::APP_DETACH:
		case UsbDfuStandardState::DFU_MANIFEST_WAIT_RESET:
			std::cout << "Resetting the device" << std::endl;
			libusb_reset_device(dfuDevice->handle);
			break;

		default:
			break;
		}
	}

	if (dfuStatus->state != UsbDfuStandardState::DFU_IDLE)
	{
		throw std::runtime_error("[ERROR_MESSAGE]Failed to idle the USB DFU target!");
	}
}

void UsbDfuComm::dfuGetStatus()
{
	// Initialize the dfuStatus before getting the actual status
	dfuStatus->pollTimeout = 0;
	dfuStatus->status = UsbDfuStandardStatus::ERROR_UNKNOWN;
	dfuStatus->state = UsbDfuStandardState::DFU_ERROR;
	dfuStatus->string = 0;

	// Request the status, state, polltime, and string from USB DFU device 
	dfuTransferIn(UsbDfuStandardProperties::REQUEST_TRANSFER_IN,
		          UsbDfuStandardCommand::GETSTATUS,
		          0,
				  (uint8_t*)&rxBuffer[0],
				  UsbDfuResponseSize::GET_STATUS);

	dfuStatus->status = rxBuffer.at(0);
	dfuStatus->pollTimeout = rxBuffer.at(1) |
		                     ((uint32_t)rxBuffer.at(2) << 8) |
		                     ((uint32_t)rxBuffer.at(3) << 16);
	dfuStatus->state = rxBuffer.at(4);
	dfuStatus->string = rxBuffer.at(5);

	// If operation status return error, throw exception
	if (dfuStatus->status != UsbDfuStandardStatus::OK)
	{
		throw std::runtime_error(translateDfuStatusToString(dfuStatus->status));
	}
}

void UsbDfuComm::dfuClearStatus()
{
	dfuTransferOut(UsbDfuStandardCommand::CLRSTATUS,
		           0,
		           NULL,
		           0);
}

void UsbDfuComm::dfuDetach()
{
	dfuTransferOut(UsbDfuStandardCommand::DETACH,
		           0,
		           NULL,
		           0);
}

void UsbDfuComm::dfuMsp432eSpecificCommandClass()
{
	dfuTransferIn(Msp432eUsbDfuProperties::REQUEST_TYPE,
		          Msp432eUsbDfuProperties::REQUEST_MSP432E,
		          Msp432eUsbDfuProperties::REQUEST_MSP432_VALUE,
		          &rxBuffer[0],
		          UsbDfuResponseSize::MSP432E_SPEC_COMMAND_CLASS);

	// Validate the MSP432E USB DFU version
	if (util->concatenateBytesToShort(&rxBuffer, 0) == Msp432eUsbDfuProperties::MSP432E_MARKER &&
		util->concatenateBytesToShort(&rxBuffer, 2) == Msp432eUsbDfuProperties::MSP432E_VERSION)
	{
		return;
	}
	else
	{
		throw std::runtime_error("[ERROR_MESSAGE]USB DFU device does not support the MSP432E USB DFU Specific command!");
	}
}

void UsbDfuComm::dfuGetDeviceInfo()
{
	txBuffer.clear();
	txBuffer.push_back(Msp432ESpecificCommand::INFO);
	for (uint8_t i = 0; i < (Msp432ECommandSize::SIZE - Msp432ECommandSize::SIZE_INFO); i++)
	{
		txBuffer.push_back(Msp432ESpecificCommand::RESERVED_VALUE);
	}

	dfuMakeIdle(false);

	dfuTransferOut(UsbDfuStandardCommand::DNLOAD,
		           0,
				   (uint8_t *)&txBuffer[0],
				   Msp432ECommandSize::SIZE);

	dfuMakeIdle(false);

	if (dfuStatus->state == UsbDfuStandardState::DFU_IDLE)
	{
		dfuTransferIn(UsbDfuStandardProperties::REQUEST_TRANSFER_IN,
			          UsbDfuStandardCommand::UPLOAD,
			          0,
			          (uint8_t*)&rxBuffer[0],
					  UsbDfuResponseSize::MSP432E_DEVICE_INFO);

		msp432eDeviceInfo->flashBlock = util->concatenateBytesToShort(&rxBuffer, 0);
		msp432eDeviceInfo->numFlashBlock = util->concatenateBytesToShort(&rxBuffer, 2);
		msp432eDeviceInfo->partInfo = util->concatenateBytesToUlong(&rxBuffer, 4);
		msp432eDeviceInfo->classInfo = util->concatenateBytesToUlong(&rxBuffer, 8);
		msp432eDeviceInfo->flashTop = util->concatenateBytesToUlong(&rxBuffer, 12);
		msp432eDeviceInfo->appStartAddr = util->concatenateBytesToUlong(&rxBuffer, 16);
	}
}