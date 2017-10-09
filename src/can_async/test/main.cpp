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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scope_exit.hpp>

#include "../../utils/Logger.h"

#include "../../can/CanMessage.h"
#include "../../can/CanAdapter.h"

#include "CanAsyncTest.h"

void log_can_error(SharedCanAdapter &can, std::string preamble){
	int errorCode = can->getErrorCode();
	std::string errorString;
	int32_t secondaryCode;
	can->getErrorDescription(errorCode, errorString, &secondaryCode);
	LOG(logERROR) << preamble << errorString;
}

int main(int argc, char* argv[])
{
	Logger *log = Logger::Instance();
	BOOST_SCOPE_EXIT(&log)
	{
		delete log;
	} BOOST_SCOPE_EXIT_END

	Logger::ReportingLevel() = logINFO;
	LOG(logINFO) << "Testing Async CAN";

	CanAdapter::CanAdapterType type = CanAdapter::SLCan;
	//CanAdapter::CanAdapterType type = CanAdapter::PeakCan;

	// display available ports
	std::string name;
	if(CanAdapter::getFirstChannelName(type, name)){
		LOG(logINFO) << "Found: " << name << std::endl;
		while(CanAdapter::getNextChannelName(type, name)){
			LOG(logINFO) << "Found: " << name << std::endl;
		}
	}

	// attempt to configure and open port
	SharedCanAdapter can = CanAdapter::getInstance(type, "COM48");
	//SharedCanAdapter can = CanAdapter::getInstance(type, "/dev/cu.usbserial-LW1ZP54X");
	//SharedCanAdapter can = CanAdapter::getInstance(type, "can0");
	//SharedCanAdapter can = CanAdapter::getInstance(type, "/dev/serial/by-id/usb-LAWICEL_CANUSB_LW1ZP54X-if00-port0");

	if(!can->setBaudRate(500000)){
		log_can_error(can, std::string("Unable to set baudrate: "));
		return EXIT_FAILURE;
	}

	if(!can->setAcceptanceFilter(0, 0x778, 0x7FF, false)){
		log_can_error(can, std::string("Unable to configure filter: "));
		return EXIT_FAILURE;
	}

	if(!can->open()){
		log_can_error(can, std::string("Unable to open port: "));
		return EXIT_FAILURE;
	}

	if(!can->goBusOn()){
		log_can_error(can, std::string("Unable to go bus-on: "));
		return EXIT_FAILURE;
	}

	CanAsyncTest test(boost::posix_time::milliseconds(2000));
	if(!test.start(can)){
		LOG(logERROR) << "Unable to start test";
		return EXIT_FAILURE;
	}

	test.stopAfterReceive();
	LOG(logINFO) << "Test done.";

	return EXIT_SUCCESS;
}
