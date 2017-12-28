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
#include <cstddef>

#include "../can/CanAdapter.h"

#pragma once

template <typename Service>
class CanAsyncIoObject
	: public boost::asio::basic_io_object<Service>
{
public:
	explicit CanAsyncIoObject(boost::asio::io_service &io_service)
	: boost::asio::basic_io_object<Service>(io_service)
	{
	}

	bool open(SharedCanAdapter aCanAdapter){
		return this->service.open(this->implementation, aCanAdapter);
	}

	void close(){
		this->service.close(this->implementation);
	}

	bool sendMessage(const SharedCanMessage &aMsg){
		return this->service.sendMessage(this->implementation, aMsg);
	}

	bool getReceivedMessage(SharedCanMessage &aMsg, uint32_t aTimeoutMs=0){
		return this->service.getReceivedMessage(this->implementation, aMsg, aTimeoutMs);
	}

	bool getSentMessage(SharedCanMessage &aMsg, uint32_t aTimeoutMs=0){
		return this->service.getSentMessage(this->implementation, aMsg, aTimeoutMs);
	}

	template <typename Handler>
	void asyncGetReceivedMessage(Handler handler)
	{
		this->service.asyncGetReceivedMessage(this->implementation, handler);
	}

	template <typename Handler>
	void asyncGetSentMessage(Handler handler)
	{
		this->service.asyncGetSentMessage(this->implementation, handler);
	}
};



