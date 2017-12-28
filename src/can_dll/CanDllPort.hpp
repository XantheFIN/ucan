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

#ifndef CAN_DLL_PORT_HPP_
#define CAN_DLL_PORT_HPP_

#ifdef SCONS_TARGET_WIN
#include <windows.h>
#endif
#include <vector>

#include "CanPort.h"
#include "can.h"
#include "CanDllWrapper.hpp"

class CanDllWrapper;

class CanDllPort : public CanPort {

public:
	CanDllPort(CanDllWrapper *aWrapper, int aHandle);
	virtual ~CanDllPort();

	/* Interface implementation */
	bool sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId);

	/* Interface implementation */
	int numReceivedMessagesAvailable();

	/* Interface implementation */
	bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);

	/* Interface implementation */
	int numSentMessagesAvailable();

	/* Interface implementation */
	bool getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);

	/* Interface implementation */
	int getErrorCode();

	/* Interface implementation */
	void getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode);

	/* Interface implementation */
	void getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter);

private:
	CanDllWrapper *mWrapper;
	int mHandle;

};

inline CanDllPort::CanDllPort(CanDllWrapper *aWrapper, int mHandle):
mWrapper(aWrapper), mHandle(mHandle){
}

inline CanDllPort::~CanDllPort(){
}

inline bool CanDllPort::sendMessage(SharedCanMessage aMsg, uint32_t aTimeoutMs){
	CAN_CanMessage m;
	m.id = aMsg->getId();
	m.flags = 0;
	if(aMsg->isExtended()){
		m.flags |= CAN_FLAG_IS_EXTENDED;
	}
	m.len = aMsg->getId();
	for(int i=0; i<aMsg->getId(); i++){
		m.data[i] = aMsg->getData(i);
	}
	return (mWrapper->sendMessage(mHandle, &m, aTimeoutMs) == 1);
}

inline int CanDllPort::numReceivedMessagesAvailable(){
	return mWrapper->numReceivedMessagesAvailable(mHandle);
}

inline bool CanDllPort::getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	CAN_CanMessage msgS;
	if(mWrapper->getReceivedMessage(mHandle, &msgS, aTimeoutMs) == 0){
		return false;
	}

	aMsg = CanMessage::getSharedInstance(msgS.id, msgS.len, (msgS.flags & CAN_FLAG_IS_EXTENDED));
	for(int i=0; i<msgS.len; i++){
		aMsg->setData(i, msgS.data[i]);
	}
	return true;
}

inline int CanDllPort::numSentMessagesAvailable(){
	return mWrapper->numSentMessagesAvailable(mHandle);
}

inline bool CanDllPort::getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	CAN_CanMessage msgS;
	if(mWrapper->getSentMessage(mHandle, &msgS, aTimeoutMs) == 0){
		return false;
	}

	aMsg = CanMessage::getSharedInstance(msgS.id, msgS.len, (msgS.flags & CAN_FLAG_IS_EXTENDED));
	for(int i=0; i<msgS.len; i++){
		aMsg->setData(i, msgS.data[i]);
	}
	return true;
}

inline int CanDllPort::getErrorCode(){
	return mWrapper->getErrorCode(mHandle);
}

inline void CanDllPort::getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode){
	char errorS[256];

	mWrapper->getErrorDescription(mHandle, aErrorCode,  errorS,  256, aSecondaryCode);

	std::string str(errorS);
	aErrorString = str;
}

inline void CanDllPort::getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter){
	mWrapper->getErrorCounters(mHandle, aTxErrorCounter, aRxErrorCounter);
}

#endif /* CAN_DLL_PORT_HPP_ */
