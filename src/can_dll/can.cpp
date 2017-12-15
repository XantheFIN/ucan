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

#include <stddef.h>

#include "../utils/Logger.h"

#include "can.h"
#include "CanAdapterManager.h"

CanAdapterManager *Manager;

void CAN_load(const char *dllNamePath){
	Logger::ReportingLevel() = logERROR;
	Manager = new CanAdapterManager();
	LOG(logINFO) << "can load()";
}

void CAN_unload(void){
	LOG(logINFO) << "can unload()";
	delete Manager;
	delete Logger::Instance();
}

int CAN_getDllVersion(){
	return(CAN_DLL_VERSION);
}

enum CanAdapter::CanAdapterType jcConvertCanAdapterType(CAN_AdapterType aType){
	switch(aType){
		default:
			return(CanAdapter::None);
		case CAN_Echo:
			return(CanAdapter::Echo);
		case CAN_Ni:
			return(CanAdapter::NiCan);
		case CAN_Lawicel:
			return(CanAdapter::LawicelCan);
		case CAN_Peak:
			return(CanAdapter::PeakCan);
		case CAN_Kvaser:
			return(CanAdapter::KvaserCan);
		case CAN_Vector:
			return(CanAdapter::VectorCan);
		case CAN_EmsWuensche:
			return(CanAdapter::EmsWuenscheCan);
		case CAN_SLCan:
			return(CanAdapter::SLCan);
		case CAN_SocketCan:
			return(CanAdapter::SocketCan);
		case CAN_NiXnetCan:
			return(CanAdapter::NiXnetCan);
	}
}

int CAN_getFirstChannelName(CAN_AdapterType aType, char* aString, int aStringLength){
	enum CanAdapter::CanAdapterType type = jcConvertCanAdapterType(aType);

	std::string name = "";
	bool success = false;
	if(type != CanAdapter::None){
		success = Manager->getFirstChannelName(type, name);
	}
	if(success){
		size_t len = name.copy(aString, aStringLength-1);
		aString[len] = '\0';
		return(1);
	} else {
		return(0);
	}
}

int CAN_getNextChannelName(CAN_AdapterType aType, char* aString, int aStringLength){
	enum CanAdapter::CanAdapterType type = jcConvertCanAdapterType(aType);

	std::string name = "";
	bool success = false;
	if(type != CanAdapter::None){
		success = Manager->getNextChannelName(type, name);
	}
	if(success){
		size_t len = name.copy(aString, aStringLength-1);
		aString[len] = '\0';
		return(1);
	} else {
		return(0);
	}
}

int CAN_obtainHandle(CAN_AdapterType aType, const char *aChannelName){
	enum CanAdapter::CanAdapterType type = jcConvertCanAdapterType(aType);

	std::string name(aChannelName);
	if(type != CanAdapter::None){
		return Manager->obtainHandle(type, name);
	} else {
		return(0);
	}
}

int CAN_handleExists(int aHandle){
	return Manager->handleExists(aHandle);
}

void CAN_releaseHandle(int aHandle){
	Manager->releaseHandle(aHandle);
}

void CAN_releaseAllHandles(){
	Manager->releaseAllHandles();
}

int CAN_setParameter(int aHandle, const char* aKey, const char* aValue){
	std::string key(aKey);
	std::string value(aValue);
	return Manager->adapter(aHandle)->setParameter(key, value);
}

int CAN_setBaudRate(int aHandle, uint32_t aBaudrate){
	return Manager->adapter(aHandle)->setBaudRate(aBaudrate);
}

int CAN_getNumberOfFilters(int aHandle){
	return Manager->adapter(aHandle)->getNumberOfFilters();
}

int CAN_setAcceptanceFilter(int aHandle, int aFid, uint32_t aCode, uint32_t aMask, int aIsExt){
	return Manager->adapter(aHandle)->setAcceptanceFilter(aFid, aCode, aMask, (aIsExt != 0));
}

int CAN_open(int aHandle){
	return Manager->adapter(aHandle)->open();
}

int CAN_goBusOn(int aHandle){
	return Manager->adapter(aHandle)->goBusOn();
}

int CAN_goBusOff(int aHandle){
	return Manager->adapter(aHandle)->goBusOff();
}

int CAN_sendMessage(int aHandle, CAN_CanMessage *aMsg, uint32_t aTimeoutMs){
	if(aMsg->flags & CAN_FLAG_IS_REMOTE_FRAME){
		// don't know what to do with this
		return(false);
	}
	SharedCanMessage msg = CanMessage::getSharedInstance(aMsg->id, aMsg->len, (aMsg->flags & CAN_FLAG_IS_EXTENDED));
	for(int i=0; i<aMsg->len; i++){
		msg->setData(i, aMsg->data[i]);
	}
	return Manager->adapter(aHandle)->sendMessage(msg, aTimeoutMs);
}

int CAN_numReceivedMessagesAvailable(int aHandle){
	return Manager->adapter(aHandle)->numReceivedMessagesAvailable();
}

int CAN_getReceivedMessage(int aHandle, CAN_CanMessage *aMsg, uint32_t aTimeoutMs){
	SharedCanMessage msg;
	if(!Manager->adapter(aHandle)->getReceivedMessage(msg, aTimeoutMs)){
		return(false);
	}
	aMsg->id = msg->getId();
	aMsg->len = msg->getLen();
	for(int i=0; i<msg->getLen(); i++){
		aMsg->data[i] = msg->getData(i);
	}
	aMsg->flags = 0;
	if(msg->isExtended()){
		aMsg->flags |= CAN_FLAG_IS_EXTENDED;
	}
	return(true);
}

int CAN_numSentMessagesAvailable(int aHandle){
	return Manager->adapter(aHandle)->numSentMessagesAvailable();
}

int CAN_getSentMessage(int aHandle, CAN_CanMessage *aMsg, uint32_t aTimeoutMs){
	SharedCanMessage msg;
	if(!Manager->adapter(aHandle)->getSentMessage(msg, aTimeoutMs)){
		return(false);
	}
	aMsg->id = msg->getId();
	aMsg->len = msg->getLen();
	for(int i=0; i<msg->getLen(); i++){
		aMsg->data[i] = msg->getData(i);
	}
	aMsg->flags = 0;
	if(msg->isExtended()){
		aMsg->flags |= CAN_FLAG_IS_EXTENDED;
	}
	return(true);
}

void CAN_close(int aHandle){
	Manager->adapter(aHandle)->close();
}

int CAN_getState(int aHandle){
	return Manager->adapter(aHandle)->getState();
}

int CANgetErrorCode(int aHandle){
	return Manager->adapter(aHandle)->getErrorCode();
}

void CAN_getErrorDescription(int aHandle, int aErrorCode, char* aString, int aStringLength, int32_t *aSecondaryCode){
	std::string errorString;
	Manager->adapter(aHandle)->getErrorDescription(aErrorCode, errorString, aSecondaryCode);
	size_t len = errorString.copy(aString, aStringLength-1);
	aString[len] = '\0';
}

void CAN_getErrorCounters(int aHandle, int *aTxErrorCounter, int *aRxErrorCounter){
	return Manager->adapter(aHandle)->getErrorCounters(aTxErrorCounter, aRxErrorCounter);
}

