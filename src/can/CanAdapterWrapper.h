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

#ifndef CAN_ADAPTER_LOADER_H_
#define CAN_ADAPTER_LOADER_H_

#include "CanAdapter.h"
#include "NullCanAdapter.h"

class CanAdapterWrapper {
	typedef CanAdapter* (*DllCreateCanAdapterInstanceFcn)(std::string);
	typedef bool (*DllGetFirstChannelNameFcn)(std::string&);
	typedef bool (*DllGetNextChannelNameFcn)(std::string&);

public:
	CanAdapterWrapper(std::string aDllName);
	virtual ~CanAdapterWrapper();

	bool load();
	void unload();

	CanAdapter* createCanAdapterInstance(std::string aChannelName){
		return mCreateCanAdapterInstanceFcn(aChannelName);
	}

	bool getFirstChannelName(std::string &aName){
		return(mGetFirstChannelNameFcn(aName));
	}

	bool getNextChannelName(std::string &aName){
		return(mGetNextChannelNameFcn(aName));
	}

private:
	bool allPointersAreNonNull();
	void setDefaultPointers();

	static CanAdapter* defaultCreateCanAdapterInstance(std::string aChannelName){
		return new NullCanAdapter();
	}

	static bool defaultGetFirstChannelName(std::string &aName){
		return(false);
	}

	static bool defaultGetNextChannelName(std::string &aName){
		return(false);
	}

	std::string mDllName;
	void* mHandle;

	DllCreateCanAdapterInstanceFcn mCreateCanAdapterInstanceFcn;
	DllGetFirstChannelNameFcn mGetFirstChannelNameFcn;
	DllGetNextChannelNameFcn mGetNextChannelNameFcn;
};

#endif /* CAN_ADAPTER_LOADER_H_ */
