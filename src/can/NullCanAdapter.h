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

#ifndef NULL_CAN_ADAPTER_H_
#define NULL_CAN_ADAPTER_H_

#include "CanMessage.h"
#include "CanAdapter.h"

class NullCanAdapter : public CanAdapter {

public:
	NullCanAdapter(){};
	virtual ~NullCanAdapter(){};

	/* Interface implementation */
	bool setParameter(std::string aKey, std::string aValue){ return false; };

	/* Interface implementation */
	bool setBaudRate(uint32_t aBaudrate){ return false; };

	/* Interface implementation */
	int getNumberOfFilters(){ return 0; };

	/* Interface implementation */
	bool setAcceptanceFilter(int aFid, uint32_t aCode, uint32_t aMask, bool aIsExt){ return false; };

	/* Interface implementation */
	bool open(){ return false; };

	/* Interface implementation */
	bool goBusOn(){ return false; };

	/* Interface implementation */
	bool goBusOff(){ return false; };

	/* Interface implementation */
	bool sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId){ return false; };

	/* Interface implementation */
	int numReceivedMessagesAvailable(){ return 0; };

	/* Interface implementation */
	bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){ return false; };

	/* Interface implementation */
	int numSendAcknMessagesAvailable(){ return 0; };

	/* Interface implementation */
	bool getSendAcknMessage(SharedCanMessage& aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs){ return false; };

	/* Interface implementation */
	void close(){ return; };

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
};

#endif /* NULL_CAN_ADAPTER_H_ */
