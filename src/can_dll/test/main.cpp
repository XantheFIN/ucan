/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Copyright 2005-2017 by CodeSkin LLC, www.codeskin.com.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * ERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <boost/scope_exit.hpp>

#include "../CanDllWrapper.hpp"

void log_can_error(CanDllWrapper &can, int canHandle, std::string preamble){
	int errorCode = can.getErrorCode(canHandle);
	char errorDescription[1000];
	int32_t secondaryCode;
	can.getErrorDescription(canHandle, errorCode, &errorDescription[0], 1000, &secondaryCode);
	std::cout << preamble << errorDescription << std::endl;
}

int main(int argc, char* argv[]) {

	CanDllWrapper can;
	if(can.loadDll() != 0){
		std::cout << "Unable to load can DLL" << std::endl;
		return EXIT_FAILURE;
	}

	CAN_AdapterType canAdapter = CAN_SLCan;

	int canHandle = can.obtainHandle(canAdapter, "COM48");
	if(canHandle == 0){
		std::cout << "Unable to get CAN handle" << std::endl;
		return EXIT_FAILURE;
	}

	BOOST_SCOPE_EXIT(&can, &canHandle)
	{
		can.releaseHandle(canHandle);
	} BOOST_SCOPE_EXIT_END

	if(!can.setBaudRate(canHandle, 500000)){
		log_can_error(can, canHandle, std::string("Unable to set baudrate: "));
		return EXIT_FAILURE;
	}

	if(!can.setAcceptanceFilter(canHandle, 0, 0x778, 0x7FF, false)){
		log_can_error(can, canHandle, std::string("Unable to configure filter: "));
		return EXIT_FAILURE;
	}

	if(!can.open(canHandle)){
		log_can_error(can, canHandle, std::string("Unable to open port: "));
		return EXIT_FAILURE;
	}

	if(!can.goBusOn(canHandle)){
		log_can_error(can, canHandle, std::string("Unable to go bus-on: "));
		return EXIT_FAILURE;
	}

	CAN_CanMessage m1, m2;
	m1.id = 0x770;
	m1.len = 8;
	m1.flags = 0;
	m1.data[0] = 0x02;
	m1.data[1] = 0x3E;
	m1.data[2] = 0x00;
	m1.data[3] = 0xFF;
	m1.data[4] = 0xFF;
	m1.data[5] = 0xFF;
	m1.data[6] = 0xFF;
	m1.data[7] = 0xFF;

	if(!can.sendMessage(canHandle, &m1, 0)){
		std::cout << "Unable to send message!" << std::endl;
	} else {
		if(!can.getReceivedMessage(canHandle, &m2, 2000)){
			std::cout << "Unable to receive message!" << std::endl;
		} else {
			std::cout << "Received message 0x" << std::hex << m2.id << std::dec << " of length " << m2.len << std::endl;
		}
	}

	can.close(canHandle);

	std::cout << "Test done" << std::endl;
	return EXIT_SUCCESS;
}
