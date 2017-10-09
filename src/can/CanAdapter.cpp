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

#include "CanAdapter.h"
#include "CanAdapterWrapper.h"

#include "../utils/LibName.h"

std::map<int, SharedCanAdapterWrapper> CanAdapter::mWrapperMap;

SharedCanAdapterWrapper CanAdapter::getWrapper(CanAdapterType aType){
	SharedCanAdapterWrapper wrapper;

	if(mWrapperMap.count(aType) != 0){
		// wrapper already exists
		wrapper = mWrapperMap[aType];
	} else {
		// attempt to create a new wrapper
		switch(aType){
			case NiCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("NiCan")));
				break;

			case PeakCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("PeakCan")));
				break;

			case LawicelCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("LawicelCan")));
				break;

			case KvaserCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("KvaserCan")));
				break;

			case VectorCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("VectorCan")));
				break;

			case EmsWuenscheCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("EmsWCan")));
				break;

			case SLCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("SLCan")));
				break;

			case SocketCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("SocketCan")));
				break;

			case Echo:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("EchoCan")));
				break;

			case NiXnetCan:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(LIB_NAME("NiXnetCan")));
				break;

			default:
				wrapper = SharedCanAdapterWrapper(new CanAdapterWrapper(""));
		}
		(void)wrapper->load();
		mWrapperMap[aType] = wrapper;
	}
	return(wrapper);
}

SharedCanAdapter CanAdapter::getInstance(CanAdapterType aType, std::string aChannelName){
	SharedCanAdapterWrapper wrapper = getWrapper(aType);
	SharedCanAdapter adapter = SharedCanAdapter(wrapper->createCanAdapterInstance(aChannelName));
	return(adapter);
}

bool CanAdapter::getFirstChannelName(CanAdapter::CanAdapterType aType, std::string &aName){
	SharedCanAdapterWrapper wrapper = getWrapper(aType);
	return wrapper->getFirstChannelName(aName);
}

bool CanAdapter::getNextChannelName(CanAdapter::CanAdapterType aType, std::string &aName){
	SharedCanAdapterWrapper wrapper = getWrapper(aType);
	return wrapper->getNextChannelName(aName);
}
