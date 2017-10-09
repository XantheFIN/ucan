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

#ifndef SERIAL_PORT_ENUMERATOR_H_
#define SERIAL_PORT_ENUMERATOR_H_

#include <string>
#include <vector>

class SerialPortEnumerator{
public:
	static bool enumeratePorts(std::vector<std::string>& ports);

private:
	SerialPortEnumerator();
	~SerialPortEnumerator();
	SerialPortEnumerator(const SerialPortEnumerator&);
	SerialPortEnumerator& operator=(const SerialPortEnumerator&);
};

#endif /* SERIAL_PORT_ENUMERATOR_H_ */
