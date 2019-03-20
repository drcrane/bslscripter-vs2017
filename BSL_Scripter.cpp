/*
==BSL_Scripter.cpp==
					
Copyright (c) 2015-2017 Texas Instruments Incorporated

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
/*******************************************************************************
*Main Program
*******************************************************************************/
int main(int argc, char * argv[])
{
	//Create the Util class that cointains functions used by other classes
	Util *util = new Util();

	//Create ScripterStream class that contains functions for streaming purpose
	ScripterStream *scripterStream;

	//Check number of arguments
	//  if number of arguments is only one, it executes the script mode
	//  and asked the script file as input
	if (argc == 1)
	{
		//Execute the Script mode
		scripterStream = new ScripterStream(util, false);
	}
	else
	{
		std::string tmp = argv[1];
		std::string checkextention = "";
		if (tmp.size() > 3)
		{
			checkextention = tmp.substr(tmp.size() - 3, tmp.size() - 1);
		}

		//If the argument is the txt file
		// then the script mode is executed
		if (util->compareString(checkextention, "txt"))
		{
			//Execute the Script mode
			scripterStream = new ScripterStream(util, false);
		}
		else
		{
			//Execute the CLI mode
			scripterStream = new ScripterStream(util, true);
		}
	}

	try
	{
		//Check if the util and scripterStream file is valid
		if ((scripterStream != nullptr) && (util != nullptr))
		{
			Interpreter interpreter(argc, argv, util, scripterStream);
		}
	}
	catch (std::runtime_error& e)
	{
		//Catch all the exception and stream out to the log file
		//if the scripterStream is present
		if (scripterStream != nullptr)
		{
			scripterStream->writeLogFile(e.what(), true);
		}
		else
		{
			std::cout << e.what() <<std::endl;
		}
	}

	//Delete the util and scripterStream before exit the program
	delete util;
	delete scripterStream;

	return 0;
}

