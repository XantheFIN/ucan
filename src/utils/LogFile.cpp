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
#include <string>
#include <time.h>
#include <sstream>

#include "LogFile.h"
#include "Logger.h"

LogFile::LogFile():
	mNullStream(), mDebugStream(&mNullStream), mFileStream(NULL), mLogFileName("")
{
}

LogFile::~LogFile(){
}

void LogFile::setFileName(std::string aFileName){
	if(aFileName == "std::cout"){
		mDebugStream = &std::cout;
		mLogFileName = "";
	} else {
		mLogFileName = aFileName;
	}
}

void LogFile::open(){
	if(mLogFileName != ""){
		mFileStream = new std::ofstream(mLogFileName.c_str());
		if(mFileStream->is_open()){
			mDebugStream = mFileStream;
		}
	}
}

void LogFile::close(){
	if(mFileStream != NULL){
		mDebugStream = &mNullStream;
		delete mFileStream; // will also close it
	}
}

std::ostream&  LogFile::debugStream()
{
	*mDebugStream << std::endl << Logger::currentDateTime() << ": ";
    return *mDebugStream;
}
