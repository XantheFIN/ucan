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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "../utils/Logger.h"

#include "CanAsyncWrapper.h"

class CanAsyncWrapper_p {
public:
	CanAsyncWrapper_p() : mPortIsOpen(false){
	};

   	~CanAsyncWrapper_p(){
   	};

   	SharedCanAdapter mCan;
    bool mPortIsOpen;
private:
};

CanAsyncWrapper::CanAsyncWrapper():
	pimpl(new CanAsyncWrapper_p()){
}

CanAsyncWrapper::~CanAsyncWrapper(){
    if(pimpl->mPortIsOpen){
        try {
            close();
        } catch(...){
            //don't throw from a destructor
        }
    }
}

bool CanAsyncWrapper::open(SharedCanAdapter aCanAdapter){
	if(pimpl->mPortIsOpen){
		close();
	}

	pimpl->mCan.reset();
	pimpl->mCan = aCanAdapter;
	pimpl->mPortIsOpen = true; // port is now open
	return pimpl->mPortIsOpen;
}

bool CanAsyncWrapper::isOpen(){
	return pimpl->mPortIsOpen;
}

void CanAsyncWrapper::close(){
    if(!pimpl->mPortIsOpen){
    	return;
    }
    pimpl->mPortIsOpen = false;
    pimpl->mCan.reset(); // don't hold on to pointer
}

bool CanAsyncWrapper::getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
    if(!pimpl->mPortIsOpen){
    	return false;
    }
	return pimpl->mCan->getSentMessage(aMsg, aTimeoutMs);
}

bool CanAsyncWrapper::getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
    if(!pimpl->mPortIsOpen){
    	return false;
    }
	return pimpl->mCan->getReceivedMessage(aMsg, aTimeoutMs);
}

bool CanAsyncWrapper::sendMessage(SharedCanMessage aMsg, uint32_t aTimeoutMs){
    if(!pimpl->mPortIsOpen){
    	return false;
    }
	return pimpl->mCan->sendMessage(aMsg, aTimeoutMs);
}

