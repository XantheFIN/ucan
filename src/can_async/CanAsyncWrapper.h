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

#ifndef CAN_ASYNC_WRAPPER_H_
#define CAN_ASYNC_WRAPPER_H_

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "../can/CanAdapter.h"

class CanAsyncWrapper_p;

class CanAsyncWrapper : private boost::noncopyable {
public:
	CanAsyncWrapper();
	virtual ~CanAsyncWrapper();

	bool open(SharedCanAdapter aCanAdapter);
	void close();
	bool isOpen();

	bool getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);
	bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);
    bool sendMessage(SharedCanMessage aMsg, uint32_t aTimeoutMs);

private:
	// make uncopyable (we only want to use shared pointers
	CanAsyncWrapper(const CanAsyncWrapper&);
	CanAsyncWrapper& operator=(const CanAsyncWrapper&);

private:
    boost::scoped_ptr<CanAsyncWrapper_p> pimpl;
};

#endif // CAN_ASYNC_WRAPPER_H_
