/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Copyright 2005-2018 by CodeSkin LLC, www.codeskin.com.
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

#ifndef KVASER_CAN_ADAPTER_H_
#define KVASER_CAN_ADAPTER_H_

#include <boost/thread/thread.hpp>

#include "../can/CanMessage.h"
#include "../can/CanMessageBuffer.h"
#include "../can/CanAdapter.h"

#include <canlib.h>

class KvaserCanAdapter : public CanAdapter, private boost::noncopyable {
public:
	enum ErrorCode {
		NoError = 0,
		KvaserError = 1,
		InvalidBaudrate = 2,
		UnsupportedHardware = 3,
		ChannelNotFound = 4,
		UnknownError = 5
	};

	KvaserCanAdapter(std::string aChannelName, uint32_t aBaudrate = 0);
	virtual ~KvaserCanAdapter();

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
	bool setParameter(std::string aKey, std::string aValue){ return false; };

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
	bool sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId);

	/* Interface implementation */
	int numReceivedMessagesAvailable();

	/* Interface implementation */
	bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);

	/* Interface implementation */
	int numSendAcknMessagesAvailable();

	/* Interface implementation */
	bool getSendAcknMessage(SharedCanMessage& aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs);

	/* Interface implementation */
	void close();

	/* Interface implementation */
	enum CanAdapterState getState();

	/* Interface implementation */
	int getErrorCode(){ return mErrorCode; };

	/* Interface implementation */
	void getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode);

	/* Interface implementation */
	void getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter);

private:
	enum {StringBufferSize = 256};

	static int mChannelIndex;

	bool setFiltering();
	bool setChannelAcceptance(uint32_t code, uint32_t mask, bool isExt);
	static void getKvaserErrorDescription(canStatus aStatus, std::string &aErrorString);

	std::string mChannelName;
	int mPortHandle;
	long mBtr01;

	uint32_t mAcceptanceMask;
	uint32_t mAcceptanceCode;
	bool mMaskIsForExtended;

	CanMessageBuffer mRxBuf;
	CanMessageBuffer mTxBuf;
	CanMessageBuffer mTxAckBuf;

    int mTxErrorCounter;
    int mRxErrorCounter;

	enum ErrorCode mErrorCode;
	canStatus mLastKvaserStatusCode;

	boolean mIsOpen;
	boolean mIsBusOn;

	mutable boost::mutex mHandleMutex;
	boost::condition_variable mCanEvent;

	// receive has polling thread
	bool mHaltReceiveThread;
	boost::thread mThread;

	void receive();

	// generates notifications
	void notify(unsigned int notifyEvent);
	static void CANLIBAPI notifyCallback(int hnd, void *context, unsigned int notifyEvent);
};
#endif /* KVASER_CAN_ADAPTER_H_ */
