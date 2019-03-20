/*
==TestReset.h==

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

#if defined(_WIN32) || defined(_WIN64)
	# define MSPBSL_ON_WIN
	# define BOOST_ASIO_OPTION_STORAGE DCB
#else
	# define MSPBSL_ON_LINUX
	# define BOOST_ASIO_OPTION_STORAGE termios
#endif

class TESTControl
{
private:

	uint16_t state;

public:
	TESTControl(uint16_t initState)
	{
		state = initState;
	}; // constructor

	boost::system::error_code store(BOOST_ASIO_OPTION_STORAGE& storage, boost::system::error_code& error) const
	{

#if defined(_WIN32) || defined(_WIN64)
		if (state)
		{
			storage.fRtsControl = RTS_CONTROL_DISABLE;
		}
		else
		{
			storage.fRtsControl = RTS_CONTROL_ENABLE;
		}
#elif defined ( __APPLE_ )

		//not yet implemented

#else
		// linux currently untested
		//uint16_t flags;
		//ioctl(fd, TIOCMGET, &flags);
		if (state)
		{
			//flags &= ~TIOCM_RTS;
			storage.c_cflag &= ~CRTSCTS;
		}
		else
		{
			//flags |= TIOCM_RTS;
			storage.c_cflag |= CRTSCTS;
		}
		//tcsetattr( fd, TCSANOW, storage );
		//ioctl(fd, TIOCMSET, &flags);
#endif
		return error;
	}; // store

	boost::system::error_code load(BOOST_ASIO_OPTION_STORAGE& storage, boost::system::error_code& error)
	{

#if defined(_WIN32) || defined(_WIN64)

		if (storage.fRtsControl == RTS_CONTROL_ENABLE)
		{
			state = 0;
		}
		else
		{
			state = 1;
		}

#elif defined ( __APPLE__ )
		
		//not implemented yet

#else
		//uint16_t flags;
		//ioctl(fd, TIOCMGET, &flags);
		//if( flags & TIOCM_RTS )
		if (storage.c_cflag & CRTSCTS)
		{
			state = 0;
		}
		else
		{
			state = 1;
	}
#endif
		return error;
	};
};

class RESETControl
{
private:

	uint16_t state;

public:
	RESETControl(uint16_t initState)
	{
		state = initState;
	};  // constructor
	boost::system::error_code store(BOOST_ASIO_OPTION_STORAGE& storage, boost::system::error_code& error) const
	{
#if defined( MSPBSL_ON_WIN )
		if (state)
		{
			storage.fDtrControl = DTR_CONTROL_ENABLE;
		}
		else
		{
			storage.fDtrControl = DTR_CONTROL_DISABLE;
		}
#elif defined ( MSPBSL_ON_LINUX )
		// linux currently untested
		//uint16_t flags;
		//ioctl(fd, TIOCMGET, &flags);
		if (state)
		{
			//flags  |= TIOCM_DTR;
			storage.c_cflag &= ~CRTSCTS;
		}
		else
		{
			//flags &= ~TIOCM_DTR;
			storage.c_cflag |= CRTSCTS;
		}
		//tcsetattr( fd, TCSANOW, storage );
		//ioctl(fd, TIOCMSET, &flags);

#endif
		return error;
	}; // store

	boost::system::error_code load(BOOST_ASIO_OPTION_STORAGE& storage, boost::system::error_code& error)
	{
#if defined( MSPBSL_ON_WIN )
		if (storage.fDtrControl == DTR_CONTROL_ENABLE)
		{
			state = 1;
		}
		else
		{
			state = 0;
		}

#elif defined ( MSPBSL_ON_LINUX )
		//uint16_t flags;
		//ioctl(fd, TIOCMGET, &flags);
		//if( flags & TIOCM_DTR )
		if (storage.c_cflag & CRTSCTS)
		{
			state = 1;
		}
		else
		{
			state = 0;
		}
#endif
		return error;
	}; // load

}; // class TestControl
