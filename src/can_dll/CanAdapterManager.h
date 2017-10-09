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

#ifndef CAN_ADAPTER_MANAGER_H_
#define CAN_ADAPTER_MANAGER_H_

#include "../utils/HandleManager.hpp"

#include "../can/CanAdapter.h"

#include "../can/CanMessage.h"


class CanAdapterManager{
public:
	CanAdapterManager();
	virtual ~CanAdapterManager();

	bool getFirstChannelName(CanAdapter::CanAdapterType aType, std::string &aName);
	bool getNextChannelName(CanAdapter::CanAdapterType aType, std::string &aName);
	int obtainHandle(CanAdapter::CanAdapterType aType, std::string aChannelName);
	int handleExists(int aHandle);
	void releaseHandle(int aHandle);
	void releaseAllHandles();

	SharedCanAdapter& adapter(int aHandle);

private:
	HandleManager<SharedCanAdapter> mHandleMgr;
};

#endif /* CAN_ADAPTER_MANAGER_H_ */
