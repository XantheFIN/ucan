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

#ifndef can_H_
#define can_H_

#include <stdint.h>

#if defined _WIN32
  #define DLLEXPORT __declspec(dllexport)
#elif __GNUC__ >=4
  #define DLLEXPORT __attribute__ ((visibility("default")))
#else
  #define DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CAN_DLL_VERSION 0x0030 // 0.3

#define CAN_FLAG_IS_EXTENDED 0x0001
#define CAN_FLAG_IS_REMOTE_FRAME 0x0002

typedef enum {
	CAN_Echo = 0,
	CAN_Ni = 1,
	CAN_Lawicel = 2,
	CAN_Peak = 3,
	CAN_Kvaser = 4,
	CAN_Vector = 5,
	CAN_EmsWuensche = 6,
	CAN_SLCan = 7,
	CAN_SocketCan = 8,
	CAN_NiXnetCan = 9
} CAN_AdapterType;

typedef struct  {
	uint32_t id;
	unsigned int len;
	unsigned char data[8];
	uint16_t flags;
} CAN_CanMessage;

DLLEXPORT int CAN_getDllVersion();

DLLEXPORT int CAN_getFirstChannelName(CAN_AdapterType aType, char* aString, int aStringLength);
DLLEXPORT int CAN_getNextChannelName(CAN_AdapterType aType, char* aString, int aStringLength);

DLLEXPORT int CAN_obtainHandle(CAN_AdapterType aType, const char *aChannelName);
DLLEXPORT int CAN_handleExists(int aHandle);
DLLEXPORT void CAN_releaseHandle(int aHandle);
DLLEXPORT void CAN_releaseAllHandles();

DLLEXPORT int CAN_setParameter(int aHandle, const char* aKey, const char* aValue);
DLLEXPORT int CAN_setBaudRate(int aHandle, uint32_t aBaudrate);
DLLEXPORT int CAN_getNumberOfFilters(int aHandle);
// convention for mask: 1 = relevant
DLLEXPORT int CAN_setAcceptanceFilter(int aHandle, int aFid, uint32_t aCode, uint32_t aMask, int IsExt);
DLLEXPORT int CAN_open(int aHandle);
DLLEXPORT int CAN_goBusOn(int aHandle);
DLLEXPORT int CAN_goBusOff(int aHandle);
DLLEXPORT int CAN_sendMessage(int aHandle, CAN_CanMessage *aMsg, uint16_t *aTransactionId);
DLLEXPORT int CAN_numReceivedMessagesAvailable(int aHandle);
DLLEXPORT int CAN_getReceivedMessage(int aHandle, CAN_CanMessage *aMsg, uint32_t aTimeoutMs);
DLLEXPORT int CAN_numSentMessagesAvailable(int aHandle);
DLLEXPORT int CAN_getSentMessage(int aHandle, CAN_CanMessage *aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs);
DLLEXPORT void CAN_close(int aHandle);

DLLEXPORT int CAN_getState(int aHandle);
DLLEXPORT int CANgetErrorCode(int aHandle);
DLLEXPORT void CAN_getErrorDescription(int aHandle, int aErrorCode, char* aString, int aStringLength, int32_t *aSecondaryCode);
DLLEXPORT void CAN_getErrorCounters(int aHandle, int *aTxErrorCounter, int *aRxErrorCounter);

#ifdef __cplusplus
}
#endif

#endif /* can_H_ */
