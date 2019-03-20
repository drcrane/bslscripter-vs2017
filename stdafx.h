/*
==stdafx.h==

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

#if defined(_WIN32) || defined(_WIN64)
	#include "targetver.h"
#else

#endif

#if defined(_WIN32) || defined(_WIN64)
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#define NOMINMAX
	#include <windows.h>
	#include <mmsystem.h>
	#define OVERRIDE override
#else
	#define OVERRIDE
#endif


#include <iostream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <cwchar>
#include <memory>
#include <csignal>

#define BOOST_DATE_TIME_NO_LIB
#define BOOST_REGEX_NO_LIB

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/utility.hpp>
#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>

#include "ThirdParty/include/libusb.h"

#include "Version.h"
#include "Enum.h" 
#include "ColorText.h"
#include "DataType.h"
#include "Util.h"
#include "ScripterStream.h"
#include "TestReset.h"
#include "TargetInterface.h"
#include "CommInterface.h"
#include "BaseCommands.h"
#include "BslResponseHandler.h"
#include "BslE4xxResponseHandler.h"
#include "BslE4xxEthernetResponseHandler.h"
#include "Family16bit.h"
#include "Family32bit.h"
#include "Family32bit_E4xx.h"
#include "Family32bit_E4xxEthernet.h"
#include "Family32bit_E4xxUsbDfu.h"
#include "FactoryTarget.h"
#include "FactoryCommunication.h"
#include "UartComm.h"
#include "UartE4xxComm.h"
#include "UsbComm.h"
#include "EthernetComm.h"
#include "UsbDfuComm.h"
#include "Timer.h"
#include "TestReset.h"
#include "Interpreter.h"	

