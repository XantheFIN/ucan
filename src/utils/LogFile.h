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

#ifndef LOGFILE_H_
#define LOGFILE_H_

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

class LogFile {
public:
	LogFile();
	virtual ~LogFile();

	void setFileName(std::string aFileName);
	void open();
	void close();
	std::ostream& debugStream();

private:
	std::ofstream mNullStream;
	std::ofstream *mFileStream;
	std::ostream *mDebugStream;
	std::string mLogFileName;
};

#endif /* LOGFILE_H_ */
