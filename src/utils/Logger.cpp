/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Based on "Logging In C++"
 * By Petru Marginean, September 05, 2007
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
#include <string>
#include <time.h>
#include <sstream>

#include "Logger.h"

Logger* Logger::mInstance = NULL;
LogLevel Logger::mLogLevel = logERROR;
const char* Logger::logLevelString[] = LOG_LEVEL_STRINGS;

Logger::Logger(){
}

Logger::~Logger(){
	mOs << std::endl;
	fprintf(stderr, "%s", mOs.str().c_str());
	fflush(stderr);
}

Logger* Logger::Instance(){
   if (!mInstance){   // Only allow one instance of class to be generated.
      mInstance = new Logger;
   }
   return mInstance;
}

LogLevel& Logger::ReportingLevel(){
	return Logger::mLogLevel;
}


const std::string  Logger::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

std::ostringstream& Logger::Get(LogLevel level){
   mOs << std::endl << "- " << currentDateTime() << "\t";
   mOs << logLevelString[level] << "\t";
   return mOs;
}





