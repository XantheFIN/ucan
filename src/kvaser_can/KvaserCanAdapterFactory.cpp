/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Copyright 2005-2018 by CodeSkin LLC, www.codeskin.com.
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

#include "../can/CanAdapterFactory.h"
#include "KvaserCanAdapter.h"

CanAdapter* createCanAdapterInstance(std::string aChannelName){
	CanAdapter *adapter = new KvaserCanAdapter(aChannelName);
	return(adapter);
}

bool getFirstChannelName(std::string &aName){
	return KvaserCanAdapter::getFirstChannelName(aName);
}

bool getNextChannelName(std::string &aName){
	return KvaserCanAdapter::getNextChannelName(aName);
}
