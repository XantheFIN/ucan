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

#ifndef SOCKET_CAN_ADAPTER_H_
#define SOCKET_CAN_ADAPTER_H_

#include <boost/noncopyable.hpp>

#include "../can/CanMessage.h"
#include "../can/CanMessageBuffer.h"
#include "../can/CanAdapter.h"

class SocketCanAdapter_p;

class SocketCanAdapter : public CanAdapter, private boost::noncopyable {

public:
	SocketCanAdapter(std::string aChannelName, uint32_t aBaudrate = 115200);
	virtual ~SocketCanAdapter();

	/**
	 * Retrieves name of first channel.
	 * @param string to which name is written
	 * @return true of channel was found
	 */
	static bool getFirstChannelName(std::string &aName);

	/**
	 * Retrieves name subsequent channel. Call after getFirstChannelName.
	 * @param string to which name is written
	 * @return true of channel was found
	 */
	static bool getNextChannelName(std::string &aName);

	/* Interface implementation */
	bool setParameter(std::string aKey, std::string aValue);

	/* Interface implementation */
	bool setBaudRate(uint32_t aBaudrate);

	/* Interface implementation */
	int getNumberOfFilters();

	/* Interface implementation */
	bool setAcceptanceFilter(int aFid, uint32_t aCode, uint32_t aMask, bool aIsExt);

	/* Interface implementation */
	bool open();

	/* Interface implementation */
	bool goBusOn();

	/* Interface implementation */
	bool goBusOff();

	/* Interface implementation */
	bool sendMessage(SharedCanMessage aMsg, uint32_t aTimeoutMs);

	/* Interface implementation */
	int numReceivedMessagesAvailable();

	/* Interface implementation */
	bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);

	/* Interface implementation */
	int numSentMessagesAvailable();

	/* Interface implementation */
	bool getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);

	/* Interface implementation */
	void close();

	/* Interface implementation */
	enum CanAdapterState getState(){ return CanAdapter::Closed; };

	/* Interface implementation */
	int getErrorCode(){ return 0; };

	/* Interface implementation */
	void getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode){
		aErrorString = "";
		*aSecondaryCode = 0;
	}

	/* Interface implementation */
	void getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter){
		*aTxErrorCounter = -1;
		*aRxErrorCounter = -1;
	}

private:
	SocketCanAdapter_p *pimpl;
};

#endif /* SOCKET_CAN_ADAPTER_H_ */
