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

#ifndef CAN_DLL_WRAPPER_HPP_
#define CAN_DLL_WRAPPER_HPP_

#ifdef SCONS_TARGET_WIN
#include <windows.h>
#endif
#ifdef SCONS_TARGET_UNIX
#include <dlfcn.h>
#endif

#include <vector>

#include "../utils/LibName.h"
#include "can.h"

#define C5_CAN_DLL_NAME LIB_NAME("Can")

class CanDllWrapper_p;

class CanDllWrapper {

public:
	CanDllWrapper();
	virtual ~CanDllWrapper();

	int loadDll();
	void unloadDll();

	int getDllVersion();

	int getFirstChannelName(int aType, char* aString, int aStringLength);
	int getNextChannelName(int aType, char* aString, int aStringLength);
	int obtainHandle(int aType, const char *aChannelName);
	int handleExists(int aHandle);
	void releaseHandle(int aHandle);
	void releaseAllHandles();

	int setParameter(int aHandle,  const char *aKey, const char *aValue);
	int setBaudRate(int aHandle, uint32_t aBaudrate);
	int getNumberOfFilters(int aHandle);
	int setAcceptanceFilter(int aHandle, int aFid, uint32_t aCode, uint32_t aMask, int IsExt);
	int open(int aHandle);
	int goBusOn(int aHandle);
	int goBusOff(int aHandle);
	int sendMessage(int aHandle, CAN_CanMessage *aMsg, uint16_t *aTransactionId);
	int numReceivedMessagesAvailable(int aHandle);
	int getReceivedMessage(int aHandle, CAN_CanMessage *aMsg, uint32_t aTimeoutMs);
	int numSendAcknMessagesAvailable(int aHandle);
	int getSendAcknMessage(int aHandle, CAN_CanMessage *aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs);

	void close(int aHandle);
	int getState(int aHandle);
	int getErrorCode(int aHandle);
	void getErrorDescription(int aHandle, int aErrorCode, char* aString, int aStringLength, int32_t *aSecondaryCode);
	void getErrorCounters(int aHandle, int *aTxErrorCounter, int *aRxErrorCounter);

private:
	CanDllWrapper_p *pimpl;
};

#ifdef SCONS_TARGET_WIN
inline static void CAN_dispError(DWORD dw){
	LPVOID lpMsgBuf;

	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

	printf("Error: %s \n", lpMsgBuf);
}
#endif

class CanDllWrapper_p {
	typedef int (*DllGetDllVersionFcn)();

	typedef int (*DllGetFirstChannelNameFcn)(CAN_AdapterType, char*, int);
	typedef int (*DllGetNextChannelNameFcn)(CAN_AdapterType, char*, int);

	typedef int (*DllObtainHandleFcn)(CAN_AdapterType, const char *);
	typedef int (*DllHandleExistsFcn)(int);
	typedef void (*DllReleaseHandleFcn)(int);
	typedef void (*DllReleaseAllHandlesFcn)();

	typedef int (*DllSetParameterFcn)(int, const char*, const char*);
	typedef int (*DllSetBaudRateFcn)(int, int);
	typedef int (*DllGetNumberOfFiltersFcn)(int);
	typedef int (*DllSetAcceptanceFilterFcn)(int, int, int, int, int);
	typedef int (*DllOpenFcn)(int);
	typedef int (*DllGoBusOnFcn)(int);
	typedef int (*DllGoBusOffFcn)(int);
	typedef int (*DllSendMessageFcn)(int, CAN_CanMessage*, uint16_t *);
	typedef int (*DllNumReceivedMessagesAvailableFcn)(int);
	typedef int (*DllGetReceivedMessageFcn)(int, CAN_CanMessage*, int);
	typedef int (*DllNumSendAcknMessagesAvailableFcn)(int);
	typedef int (*DllGetSendAcknMessageFcn)(int, CAN_CanMessage*, uint16_t, int);
	typedef void (*DllCloseFcn)(int);

	typedef int (*DllGetStateFcn)(int);
	typedef int (*DllGetErrorCodeFcn)(int);
	typedef int (*DllGetErrorDescriptionFcn)(int, int, char*, int, int32_t*);
	typedef void (*DllGetErrorCountersFcn)(int, int *, int *);

public:
	CanDllWrapper_p();
	virtual ~CanDllWrapper_p();

	int loadDll();
	void unloadDll();

	int getDllVersion();

	inline DllGetDllVersionFcn getGetDllVersionFcn() const { return mDllGetDllVersionFcn; }

	inline DllGetFirstChannelNameFcn getGetFirstChannelNameFcn() const { return mDllGetFirstChannelNameFcn; }
	inline DllGetNextChannelNameFcn getGetNextChannelNameFcn() const { return mDllGetNextChannelNameFcn; }

	inline DllObtainHandleFcn getObtainHandleFcn() const { return mObtainHandleFcn; }
	inline DllHandleExistsFcn getHandleExistsFcn() const { return mHandleExistsFcn; }
	inline DllReleaseHandleFcn getReleaseHandleFcn() const { return mReleaseHandleFcn; }
	inline DllReleaseAllHandlesFcn getReleaseAllHandlesFcn() const { return mReleaseAllHandlesFcn; }

	inline DllSetParameterFcn getSetParameterFcn() const { return mDllSetParameterFcn; };
	inline DllSetBaudRateFcn getSetBaudRateFcn() const { return mSetBaudRateFcn; }
	inline DllGetNumberOfFiltersFcn getGetNumberOfFiltersFcn() const { return mGetNumberOfFiltersFcn; }
	inline DllSetAcceptanceFilterFcn getSetAcceptanceFilterFcn() const { return mSetAcceptanceFilterFcn; }
	inline DllOpenFcn getOpenFcn() const { return mOpenFcn; }
	inline DllGoBusOnFcn getGoBusOnFcn() const { return mGoBusOnFcn; }
	inline DllGoBusOffFcn getGoBusOffFcn() const { return mGoBusOffFcn; }
	inline DllSendMessageFcn getSendMessageFcn() const { return mSendMessageFcn; }
	inline DllNumReceivedMessagesAvailableFcn getNumReceivedMessagesAvailableFcn() const { return mNumReceivedMessagesAvailableFcn; }
	inline DllGetReceivedMessageFcn getGetReceivedMessageFcn() const { return mGetReceivedMessageFcn; }
	inline DllNumSendAcknMessagesAvailableFcn getNumSendAcknMessagesAvailableFcn() const { return mNumSendAcknMessagesAvailableFcn; }
	inline DllGetSendAcknMessageFcn getGetSendAcknMessageFcn() const { return mGetSendAcknMessageFcn; }
	inline DllCloseFcn getCloseFcn() const { return mCloseFcn; }

	inline DllGetStateFcn getGetStateFcn() const { return mDllGetStateFcn; }
	inline DllGetErrorCodeFcn getGetErrorCodeFcn() const { return mDllGetErrorCodeFcn; }
	inline DllGetErrorDescriptionFcn getGetErrorDescriptionFcn() const { return mDllGetErrorDescriptionFcn; }
	inline DllGetErrorCountersFcn getGetErrorCountersFcn() const { return mDllGetErrorCountersFcn; }

private:
	int allPointersAreNonNull();

	void* mHandle;

	DllGetDllVersionFcn mDllGetDllVersionFcn;

	DllGetFirstChannelNameFcn mDllGetFirstChannelNameFcn;
	DllGetNextChannelNameFcn mDllGetNextChannelNameFcn;

	DllObtainHandleFcn mObtainHandleFcn;
	DllHandleExistsFcn mHandleExistsFcn;
	DllReleaseHandleFcn mReleaseHandleFcn;
	DllReleaseAllHandlesFcn mReleaseAllHandlesFcn;

	DllSetParameterFcn mDllSetParameterFcn;
	DllSetBaudRateFcn mSetBaudRateFcn;
	DllGetNumberOfFiltersFcn mGetNumberOfFiltersFcn;
	DllSetAcceptanceFilterFcn mSetAcceptanceFilterFcn;
	DllOpenFcn mOpenFcn;
	DllGoBusOnFcn mGoBusOnFcn;
	DllGoBusOffFcn mGoBusOffFcn;
	DllSendMessageFcn mSendMessageFcn;
	DllNumReceivedMessagesAvailableFcn mNumReceivedMessagesAvailableFcn;
	DllGetReceivedMessageFcn mGetReceivedMessageFcn;
	DllNumSendAcknMessagesAvailableFcn mNumSendAcknMessagesAvailableFcn;
	DllGetSendAcknMessageFcn mGetSendAcknMessageFcn;
	DllCloseFcn mCloseFcn;

	DllGetStateFcn mDllGetStateFcn;
	DllGetErrorCodeFcn mDllGetErrorCodeFcn;
	DllGetErrorDescriptionFcn mDllGetErrorDescriptionFcn;
	DllGetErrorCountersFcn mDllGetErrorCountersFcn;
};

inline CanDllWrapper_p::CanDllWrapper_p() {
	mHandle = NULL;
}

inline CanDllWrapper_p::~CanDllWrapper_p(){
	unloadDll();
}

#ifdef SCONS_TARGET_WIN
inline void CanDllWrapper_p::unloadDll(){
	if (mHandle)
	{
		FreeLibrary((HMODULE)mHandle);
		mHandle = NULL;
	}
}

inline int CanDllWrapper_p::loadDll(){
	mHandle = LoadLibrary(C5_CAN_DLL_NAME);
	if (!mHandle){
		printf("CAN LoadLibrary failed with error: %d \n", GetLastError());
		CAN_dispError(GetLastError());
		return(-1);
	} else {

		mDllGetDllVersionFcn = (DllGetDllVersionFcn)GetProcAddress((HMODULE)mHandle, "CAN_getDllVersion");

		mDllGetFirstChannelNameFcn = (DllGetFirstChannelNameFcn)GetProcAddress((HMODULE)mHandle, "CAN_getFirstChannelName");
		mDllGetNextChannelNameFcn = (DllGetNextChannelNameFcn)GetProcAddress((HMODULE)mHandle, "CAN_getNextChannelName");

		mObtainHandleFcn = (DllObtainHandleFcn)GetProcAddress((HMODULE)mHandle, "CAN_obtainHandle");
		mHandleExistsFcn = (DllHandleExistsFcn)GetProcAddress((HMODULE)mHandle, "CAN_handleExists");

		mReleaseHandleFcn = (DllReleaseHandleFcn)GetProcAddress((HMODULE)mHandle, "CAN_releaseHandle");
		mReleaseAllHandlesFcn = (DllReleaseAllHandlesFcn)GetProcAddress((HMODULE)mHandle, "CAN_releaseAllHandles");

		mDllSetParameterFcn = (DllSetParameterFcn)GetProcAddress((HMODULE)mHandle, "CAN_setParameter");
		mSetBaudRateFcn = (DllSetBaudRateFcn)GetProcAddress((HMODULE)mHandle, "CAN_setBaudRate");
		mGetNumberOfFiltersFcn = (DllGetNumberOfFiltersFcn)GetProcAddress((HMODULE)mHandle, "CAN_getNumberOfFilters");
		mSetAcceptanceFilterFcn = (DllSetAcceptanceFilterFcn)GetProcAddress((HMODULE)mHandle, "CAN_setAcceptanceFilter");
		mOpenFcn = (DllOpenFcn)GetProcAddress((HMODULE)mHandle, "CAN_open");
		mGoBusOnFcn = (DllGoBusOnFcn)GetProcAddress((HMODULE)mHandle, "CAN_goBusOn");
		mGoBusOffFcn = (DllGoBusOffFcn)GetProcAddress((HMODULE)mHandle, "CAN_goBusOff");
		mSendMessageFcn = (DllSendMessageFcn)GetProcAddress((HMODULE)mHandle, "CAN_sendMessage");
		mNumReceivedMessagesAvailableFcn = (DllNumReceivedMessagesAvailableFcn)GetProcAddress((HMODULE)mHandle, "CAN_numReceivedMessagesAvailable");
		mGetReceivedMessageFcn = (DllGetReceivedMessageFcn)GetProcAddress((HMODULE)mHandle, "CAN_getReceivedMessage");
		mNumSendAcknMessagesAvailableFcn = (DllNumSendAcknMessagesAvailableFcn)GetProcAddress((HMODULE)mHandle, "CAN_numSendAcknMessagesAvailable");
		mGetSendAcknMessageFcn = (DllGetSendAcknMessageFcn)GetProcAddress((HMODULE)mHandle, "CAN_getSendAcknMessage");
		mCloseFcn = (DllCloseFcn)GetProcAddress((HMODULE)mHandle, "CAN_close");

		mDllGetStateFcn = (DllGetStateFcn)GetProcAddress((HMODULE)mHandle, "CAN_getState");
		mDllGetErrorCodeFcn = (DllGetErrorCodeFcn)GetProcAddress((HMODULE)mHandle, "CANgetErrorCode");
		mDllGetErrorDescriptionFcn = (DllGetErrorDescriptionFcn)GetProcAddress((HMODULE)mHandle, "CAN_getErrorDescription");
		mDllGetErrorCountersFcn = (DllGetErrorCountersFcn)GetProcAddress((HMODULE)mHandle, "CAN_getErrorCounters");

		if(!allPointersAreNonNull()){
			printf("Unable to obtain function handles. Error: %d \n", GetLastError());
			CAN_dispError(GetLastError());
			return(-2);
		}
	}
	return(0);
}
#endif
#ifdef SCONS_TARGET_UNIX
inline void CanDllWrapper_p::unloadDll(){
	if (mHandle)
	{
		dlclose(mHandle);
		mHandle = NULL;
	}
}

inline int CanDllWrapper_p::loadDll(){
	mHandle = dlopen(C5_CAN_DLL_NAME, RTLD_LAZY);
	if (!mHandle){
		printf("CAN LoadLibrary failed with error: %s \n", dlerror());
		return(-1);
	} else {

		mDllGetDllVersionFcn = (DllGetDllVersionFcn)dlsym(mHandle, "CAN_getDllVersion");

		mDllGetFirstChannelNameFcn = (DllGetFirstChannelNameFcn)dlsym(mHandle, "CAN_getFirstChannelName");
		mDllGetNextChannelNameFcn = (DllGetNextChannelNameFcn)dlsym(mHandle, "CAN_getNextChannelName");

		mObtainHandleFcn = (DllObtainHandleFcn)dlsym(mHandle, "CAN_obtainHandle");
		mHandleExistsFcn = (DllHandleExistsFcn)dlsym(mHandle, "CAN_handleExists");

		mReleaseHandleFcn = (DllReleaseHandleFcn)dlsym(mHandle, "CAN_releaseHandle");
		mReleaseAllHandlesFcn = (DllReleaseAllHandlesFcn)dlsym(mHandle, "CAN_releaseAllHandles");

		mDllSetParameterFcn = (DllSetParameterFcn)dlsym(mHandle, "CAN_setParameter");
		mSetBaudRateFcn = (DllSetBaudRateFcn)dlsym(mHandle, "CAN_setBaudRate");
		mGetNumberOfFiltersFcn = (DllGetNumberOfFiltersFcn)dlsym(mHandle, "CAN_getNumberOfFilters");
		mSetAcceptanceFilterFcn = (DllSetAcceptanceFilterFcn)dlsym(mHandle, "CAN_setAcceptanceFilter");
		mOpenFcn = (DllOpenFcn)dlsym(mHandle, "CAN_open");
		mGoBusOnFcn = (DllGoBusOnFcn)dlsym(mHandle, "CAN_goBusOn");
		mGoBusOffFcn = (DllGoBusOffFcn)dlsym(mHandle, "CAN_goBusOff");
		mSendMessageFcn = (DllSendMessageFcn)dlsym(mHandle, "CAN_sendMessage");
		mNumReceivedMessagesAvailableFcn = (DllNumReceivedMessagesAvailableFcn)dlsym(mHandle, "CAN_numReceivedMessagesAvailable");
		mGetReceivedMessageFcn = (DllGetReceivedMessageFcn)dlsym(mHandle, "CAN_getReceivedMessage");
		mNumSendAcknMessagesAvailableFcn = (DllNumSendAcknMessagesAvailableFcn)dlsym(mHandle, "CAN_numSendAcknMessagesAvailable");
		mGetSendAcknMessageFcn = (DllGetSendAcknMessageFcn)dlsym(mHandle, "CAN_getSendAcknMessage");
		mCloseFcn = (DllCloseFcn)dlsym(mHandle, "CAN_close");

		mDllGetStateFcn = (DllGetStateFcn)dlsym(mHandle, "CAN_getState");
		mDllGetErrorCodeFcn = (DllGetErrorCodeFcn)dlsym(mHandle, "CANgetErrorCode");
		mDllGetErrorDescriptionFcn = (DllGetErrorDescriptionFcn)dlsym(mHandle, "CAN_getErrorDescription");
		mDllGetErrorCountersFcn = (DllGetErrorCountersFcn)dlsym(mHandle, "CAN_getErrorCounters");

		if(!allPointersAreNonNull()){
			printf("Unable to obtain CAN function handles. Error: %s \n", dlerror());

			return(-2);
		}
	}
	return(0);
}
#endif

inline int CanDllWrapper_p::allPointersAreNonNull() {
	return(
			(mDllGetDllVersionFcn != NULL) &&

			(mDllGetFirstChannelNameFcn != NULL) &&
			(mDllGetNextChannelNameFcn != NULL) &&

			(mObtainHandleFcn != NULL) &&
			(mHandleExistsFcn != NULL) &&
			(mReleaseHandleFcn != NULL) &&
			(mReleaseAllHandlesFcn != NULL) &&

			(mDllSetParameterFcn != NULL) &&
			(mSetBaudRateFcn != NULL) &&
			(mGetNumberOfFiltersFcn != NULL) &&
			(mSetAcceptanceFilterFcn != NULL) &&
			(mOpenFcn != NULL) &&
			(mGoBusOnFcn != NULL) &&
			(mGoBusOffFcn != NULL) &&
			(mSendMessageFcn != NULL) &&
			(mNumReceivedMessagesAvailableFcn != NULL) &&
			(mGetReceivedMessageFcn != NULL) &&
			(mNumSendAcknMessagesAvailableFcn != NULL) &&
			(mGetSendAcknMessageFcn != NULL) &&
			(mCloseFcn != NULL) &&

			(mDllGetStateFcn != NULL) &&
			(mDllGetErrorCodeFcn != NULL) &&
			(mDllGetErrorDescriptionFcn != NULL) &&
			(mDllGetErrorCountersFcn != NULL)
	);
}

inline CanDllWrapper::CanDllWrapper() {
	pimpl = new CanDllWrapper_p();
}

inline CanDllWrapper::~CanDllWrapper(){
	delete pimpl;
}

inline void CanDllWrapper::unloadDll(){
	pimpl->unloadDll();
}

inline int CanDllWrapper::loadDll(){
	return pimpl->loadDll();
}

inline int CanDllWrapper::getDllVersion(){
	return pimpl->getGetDllVersionFcn()();
}

inline int CanDllWrapper::getFirstChannelName(int aType, char* aString, int aStringLength){
	return pimpl->getGetFirstChannelNameFcn()((CAN_AdapterType)aType, aString, aStringLength);
}

inline int CanDllWrapper::getNextChannelName(int aType, char* aString, int aStringLength){
	return pimpl->getGetNextChannelNameFcn()((CAN_AdapterType)aType, aString, aStringLength);
}

inline int CanDllWrapper::obtainHandle(int aType, const char *aChannelName){
	return pimpl->getObtainHandleFcn()((CAN_AdapterType)aType, aChannelName);
}

inline int CanDllWrapper::handleExists(int aHandle){
	return pimpl->getHandleExistsFcn()(aHandle);
}

inline void CanDllWrapper::releaseHandle(int aHandle){
	pimpl->getReleaseHandleFcn()(aHandle);
}

inline void CanDllWrapper::releaseAllHandles(){
	pimpl->getReleaseAllHandlesFcn()();
}

inline int CanDllWrapper::setParameter(int aHandle,  const char *aKey, const char *aValue){
	return pimpl->getSetParameterFcn()(aHandle, aKey, aValue);
}

inline int CanDllWrapper::setBaudRate(int aHandle, uint32_t aBaudrate){
	return pimpl->getSetBaudRateFcn()(aHandle, aBaudrate);
}

inline int CanDllWrapper::getNumberOfFilters(int aHandle){
	return pimpl->getGetNumberOfFiltersFcn()(aHandle);
}

inline int CanDllWrapper::setAcceptanceFilter(int aHandle, int aFid, uint32_t aCode, uint32_t aMask, int IsExt){
	return pimpl->getSetAcceptanceFilterFcn()(aHandle, aFid, aCode, aMask, IsExt);
}

inline int CanDllWrapper::open(int aHandle){
	return pimpl->getOpenFcn()(aHandle);
}

inline int CanDllWrapper::goBusOn(int aHandle){
	return pimpl->getGoBusOnFcn()(aHandle);
}

inline int CanDllWrapper::goBusOff(int aHandle){
	return pimpl->getGoBusOffFcn()(aHandle);
}

inline int CanDllWrapper::sendMessage(int aHandle, CAN_CanMessage *aMsg, uint16_t *aTransactionId){
	return pimpl->getSendMessageFcn()(aHandle, aMsg, aTransactionId);
}

inline int CanDllWrapper::numReceivedMessagesAvailable(int aHandle){
	return pimpl->getNumReceivedMessagesAvailableFcn()(aHandle);
}

inline int CanDllWrapper::getReceivedMessage(int aHandle, CAN_CanMessage *aMsg, uint32_t aTimeoutMs){
	return pimpl->getGetReceivedMessageFcn()(aHandle, aMsg,aTimeoutMs);
}

inline int CanDllWrapper::numSendAcknMessagesAvailable(int aHandle){
	return pimpl->getNumSendAcknMessagesAvailableFcn()(aHandle);
}

inline int CanDllWrapper::getSendAcknMessage(int aHandle, CAN_CanMessage *aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs){
	return pimpl->getGetSendAcknMessageFcn()(aHandle, aMsg, aTransactionId, aTimeoutMs);
}

inline void CanDllWrapper::close(int aHandle){
	return pimpl->getCloseFcn()(aHandle);
}

inline int CanDllWrapper::getState(int aHandle){
	return pimpl->getGetStateFcn()(aHandle);
}

inline int CanDllWrapper::getErrorCode(int aHandle){
	return pimpl->getGetErrorCodeFcn()(aHandle);
}

inline void CanDllWrapper::getErrorDescription(int aHandle, int aErrorCode, char* aString, int aStringLength, int32_t *aSecondaryCode){
	pimpl->getGetErrorDescriptionFcn()(aHandle, aErrorCode, aString, aStringLength, aSecondaryCode);
}

inline void CanDllWrapper::getErrorCounters(int aHandle, int *aTxErrorCounter, int *aRxErrorCounter){
	pimpl->getGetErrorCountersFcn()(aHandle, aTxErrorCounter, aRxErrorCounter);
}

#endif /* CAN_DLL_WRAPPER_HPP_ */
