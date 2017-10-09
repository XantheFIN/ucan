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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <sstream>


#define LOG(level) \
	if(level > Logger::ReportingLevel()); \
	else Logger::Instance()->Get(level)

enum LogLevel
	{logERROR, logWARNING, logINFO, logDEBUG};

#define LOG_LEVEL_STRINGS \
	{"Error", "Warning", "Info", "Debug"}

/**
 * Simple logging framework
 *
 * Really a placeholder until something more powerful
 * is put in place/adopted.
 */
class Logger{
public:
	/** Instance getter (is singleton) */
	static Logger* Instance();
	/** Getter/setter for level above which reporting is suppressed */
	static LogLevel& ReportingLevel();
	/** use LOG macro instead of this method ! */
	std::ostringstream& Get(LogLevel level);
	/** must be called for messages to be saved/displayed */

	static const std::string currentDateTime();

	virtual ~Logger();

private:
	Logger();                                  // Private constructor
	Logger(const Logger&);                 // Prevent copy-construction
	Logger& operator=(const Logger&);      // Prevent assignment

	static Logger* mInstance;
	static LogLevel mLogLevel;
	static const char* logLevelString[];

	std::ostringstream mOs;
};

#endif /* LOG_H_ */
