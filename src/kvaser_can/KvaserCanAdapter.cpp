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

/*
 * This driver has a true implementation of the tx-acknowledgment.
 */

#include <sstream>

#include "../utils/Logger.h"
#include "KvaserCanAdapter.h"

int KvaserCanAdapter::mChannelIndex = -1;

/*
 * http://www.kvaser.com/canlib-webhelp/page_user_guide_threads_applications.html
 * -> "Handles are thread specific!
 * -> we use mutex to protect access to handle
 */

KvaserCanAdapter::KvaserCanAdapter(std::string aChannelName, uint32_t aBaudrate) :
	mPortHandle(NULL),
	mHandleMutex(),
	mCanEvent(),
	mThread(),
	mRxBuf(),
	mTxBuf(),
	mTxAckBuf(),
	mIsOpen(false),
	mIsBusOn(false),
	mChannelName(aChannelName),
	mLastKvaserStatusCode(canOK),
    mTxErrorCounter(0),
    mRxErrorCounter(0),
	mAcceptanceMask(0),
	mAcceptanceCode(0),
	mMaskIsForExtended(false)
{
	canInitializeLibrary();
	(void)setBaudRate(aBaudrate); // no error checking
}

KvaserCanAdapter::~KvaserCanAdapter(){
	if(mIsOpen){
		close();
	}
}

void KvaserCanAdapter::getKvaserErrorDescription(canStatus aStatus, std::string &aErrorString){
	char buf[StringBufferSize];
	if (aStatus != canOK) {
		buf[0] = '\0';
		if(canGetErrorText(aStatus, buf, sizeof(buf)) == canOK){
			aErrorString = std::string(buf);
		} else {
			aErrorString = "Unknown error";
		}
	} else {
		aErrorString = "OK";
	}
}

bool KvaserCanAdapter::getFirstChannelName(std::string &aName){
	canInitializeLibrary();
	mChannelIndex = 0;
	return(getNextChannelName(aName));
}

bool KvaserCanAdapter::getNextChannelName(std::string &aName){
	if(mChannelIndex < 0){
		return false;
	}
	int channel_count = 0;
	canStatus status = canGetNumberOfChannels(&channel_count);
	if(status != canOK){
		return false;
	}

	while(mChannelIndex < channel_count){
		uint32_t type;
		status = canGetChannelData(mChannelIndex, canCHANNELDATA_CARD_TYPE, &type, sizeof(type));
		if(status != canOK){
			return false;
		}

		if((type == canHWTYPE_NONE) || (type == canHWTYPE_VIRTUAL)){
			return false;
		}


		uint32_t num;
		status = canGetChannelData(mChannelIndex, canCHANNELDATA_CARD_NUMBER, &num, sizeof(num));
		if(status != canOK){
			return false;
		}

		uint32_t chan;
		status = canGetChannelData(mChannelIndex, canCHANNELDATA_CHAN_NO_ON_CARD, &chan, sizeof(chan));
		if(status != canOK){
			return false;
		}

		char name[StringBufferSize];
		name[0] = '\0';
		status = canGetChannelData(mChannelIndex, canCHANNELDATA_CHANNEL_NAME, name, sizeof(name));
		if(status != canOK){
			return false;
		}

		printf("%d\t %s \t %d \t %d \t %d \n", mChannelIndex, name, type, num, chan);
		aName = std::string(name);
		mChannelIndex++;
		return true; // only one at a time
	}
	return false;
}

/* Interface implementation */
void KvaserCanAdapter::getErrorDescription(int aErrorCode, std::string &aErrorString, int32_t *aSecondaryCode){
	switch(mErrorCode){
	case NoError:
		aErrorString = "";
		*aSecondaryCode = 0;
		break;

	case KvaserError:
		getKvaserErrorDescription(mLastKvaserStatusCode, aErrorString);
		*aSecondaryCode = mLastKvaserStatusCode;
		break;

	case InvalidBaudrate:
		aErrorString = "Invalid Baudrate";
		*aSecondaryCode = 0;
		break;

	case ChannelNotFound:
		aErrorString = "Channel not found";
		*aSecondaryCode = 0;
		break;

	case UnsupportedHardware:
		aErrorString = "Unsupported Hardware";
		*aSecondaryCode = 0;
		break;

	default:
		aErrorString = "Unknown Error";
		*aSecondaryCode = 0;
		break;

	}
}

/* Interface implementation */
bool KvaserCanAdapter::setBaudRate(uint32_t aBaudrate){
	if(mIsOpen){
		return false;
	}

	if((aBaudrate & 0x80000000) ==  0x80000000){
		// special "NI" convention
		mBtr01 = ((aBaudrate & 0xFF) << 8) + ((aBaudrate & 0xFF00) >> 8);
		return true;
	} else {
		switch(aBaudrate){
		case 1000000:
			mBtr01 = 0x0014;
			break;
		case 800000:
			mBtr01 = 0x0016;
			break;
		case 500000:
			mBtr01 = 0x001C;
			break;
		case 250000:
			mBtr01 = 0x011C;
			break;
		case 125000:
			mBtr01 = 0x031C;
			break;
		case 100000:
			mBtr01 = 0x432F;
			break;
		case 50000:
			mBtr01 = 0x472F;
			break;
		case 20000:
			mBtr01 = 0x532F;
			break;
		case 10000:
			mBtr01 = 0x672F;
			break;
		case 5000:
			mBtr01 = 0x7F7F;
			break;
		default:
			mBtr01 = 0;
			mErrorCode = InvalidBaudrate;
			return false;
		}
	}
	return true;
}

/* Interface implementation */
int KvaserCanAdapter::getNumberOfFilters(){
	/*
	 * We only support one filter, which on series 2 devices allows us to
	 * use NC_FILTER_SINGLE_STANDARD or NC_FILTER_SINGLE_EXTENDED
	 */
	return(1);
}

bool KvaserCanAdapter::setChannelAcceptance(uint32_t code, uint32_t mask, bool isExt){
	mLastKvaserStatusCode = canSetAcceptanceFilter(mPortHandle, code, mask, isExt);
	return (mLastKvaserStatusCode == canOK);
}

/* not thread-safe - only to be called by open() function */
bool KvaserCanAdapter::setFiltering(){
	/*
	 * Notes regarding filtering:
	 * While the documentation is not clear, it seems that there are two masks,
	 * one for standard and a second for extended messages, that need to be set
	 * separately.
	 * - extended message have a complete bit-wise mask: 0x1FFFFFFF
	 * - standard message filter masks can also be set to 0x1FFFFFFF, but only 7FF
	 *   is relevant
     */

	// set one filter (extended or standard)
	if(!setChannelAcceptance(mAcceptanceCode, mAcceptanceMask, mMaskIsForExtended)){
		return(false);
	}
	// set other filter to only pass absolute lowest priority message
	if(mMaskIsForExtended){
		// other filter is standard
		if(!setChannelAcceptance(0x7FF, 0x7FF, false)){
			mErrorCode = KvaserError;
			return(false);
		}
	} else {
		// other filter is extended
		if(!setChannelAcceptance(0x1FFFFFFF, 0x1FFFFFFF, true)){
			mErrorCode = KvaserError;
			return(false);
		}
	}
	return true;
}

/* Interface implementation */
bool KvaserCanAdapter::setAcceptanceFilter(int fid, uint32_t code, uint32_t mask, bool isExt){
	if(fid != 0){
		// only one filter
		return(false);
	}

	if(mIsOpen){
		// filter can only be configured when port is closed
		return(false);
	}

	mAcceptanceMask = mask;
	mAcceptanceCode = code;
	mMaskIsForExtended = isExt;

	if(!isExt){
		mAcceptanceMask &= 0x7FF;
	} else {
		mAcceptanceMask &= 0x1FFFFFFF;
	}

	// actual config will happen when we open the port
	return true;
}

/* Interface implementation */
bool KvaserCanAdapter::open(){
	if(mIsOpen){
		return(false); // TODO: should we be more tolerant?
	}

	// has baudrate been configured?
	if(mBtr01 == 0){
		mErrorCode = InvalidBaudrate;
		return(false);
	}

	int channel;
	std::stringstream(mChannelName) >> channel;

	mPortHandle = canOpenChannel(channel, 0);
    if(mPortHandle < 0) {
    	mErrorCode = ChannelNotFound;
    	return false;
    }
    mLastKvaserStatusCode = canSetBusParamsC200(mPortHandle, (mBtr01>>8) & 0xFF, mBtr01 & 0xFF);
    if(mLastKvaserStatusCode != canOK){
    	mErrorCode = KvaserError;
		canClose(mPortHandle);
		return(false);
    }

    int i = 1; // Turn on txAcks
    mLastKvaserStatusCode = canIoCtl(mPortHandle, canIOCTL_SET_TXACK, &i, 4);
    if (mLastKvaserStatusCode != canOK) {
    	mErrorCode = KvaserError;
		canClose(mPortHandle);
		return(false);
    }

	if(!setFiltering()){
    	mErrorCode = KvaserError;
		canClose(mPortHandle);
		return(false);
	}

	mHaltReceiveThread = false;
	boost::thread t(boost::bind(&KvaserCanAdapter::receive, this));
	mThread.swap(t);

	mErrorCode = NoError;
	mIsOpen = true;
	return(true);
}

/* Interface implementation */
void KvaserCanAdapter::close(){
	if(!mIsOpen){
		return; // nothing to do
	}
	if(mIsBusOn){
		goBusOff();
	}
	// set flag to stop receive thread
	mHaltReceiveThread = true;
	// signal notifier to wake-up receive thread
	mCanEvent.notify_one();
	// wait for thread to complete
	mThread.join();
	// clear buffers
	mTxBuf.clear();
	mRxBuf.clear();
	mTxAckBuf.clear();

	canClose(mPortHandle);

	mIsOpen = false;
}

/* Interface implementation */
bool KvaserCanAdapter::goBusOn(){
	if(!mIsOpen){
		return(false);
	}

	if(mIsBusOn){
		return(true);
	}

	boost::mutex::scoped_lock lock(mHandleMutex);
	// also available: canNOTIFY_ERROR | canNOTIFY_STATUS
    mLastKvaserStatusCode = kvSetNotifyCallback(mPortHandle, notifyCallback, this, canNOTIFY_RX | canNOTIFY_TX);
    if(mLastKvaserStatusCode != canOK){
    	mErrorCode = KvaserError;
		return(false);
    }
	mLastKvaserStatusCode = canBusOn(mPortHandle);
	if(mLastKvaserStatusCode != canOK){
		mErrorCode = KvaserError;
		return(false);
	}
	mIsBusOn = true;
	return(true);
}

/* Interface implementation */
bool KvaserCanAdapter::goBusOff(){
	if(!mIsOpen){
		return(false);
	}

	if(!mIsBusOn){
		return(true);
	}

	boost::mutex::scoped_lock lock(mHandleMutex);
	kvSetNotifyCallback(mPortHandle, NULL, NULL, 0);
	mLastKvaserStatusCode = canBusOff(mPortHandle);
	if(mLastKvaserStatusCode != canOK){
		mErrorCode = KvaserError;
		return(false);
	}
	mErrorCode = NoError;
	mIsBusOn = false;
	return(true);
}

/* Interface implementation */
bool KvaserCanAdapter::sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId){
	if(!mIsBusOn){
		return(false);
	}

	// transaction ID not yet implemented
	if(aTransactionId != 0){
		*aTransactionId = 0;
	}
	char msg[8];
	for(int i=0; i<aMsg->getLen(); i++){
		msg[i] = aMsg->getData(i);
	}
	boost::mutex::scoped_lock lock(mHandleMutex);
	if(aMsg->isExtended()){
		mLastKvaserStatusCode = canWrite(mPortHandle, aMsg->getId(), msg, aMsg->getLen(), canMSG_EXT);
	} else {
		mLastKvaserStatusCode = canWrite(mPortHandle, aMsg->getId(), msg, aMsg->getLen(), 0);
	}
	return (mLastKvaserStatusCode == canOK);
}

/* Interface implementation */
int KvaserCanAdapter::numReceivedMessagesAvailable(){
	if(!mIsBusOn){
		return(0);
	}
	return(mRxBuf.available());
}

/* Interface implementation */
bool KvaserCanAdapter::getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	if(!mIsOpen){
		return(false);
	}
	return(mRxBuf.pop(aMsg, aTimeoutMs));
}

/* Interface implementation */
int KvaserCanAdapter::numSendAcknMessagesAvailable(){
	if(!mIsBusOn){
		return(0);
	}
	return(mTxAckBuf.available());
}

/* Interface implementation */
bool KvaserCanAdapter::getSendAcknMessage(SharedCanMessage& aMsg, uint16_t aTransactionId, uint32_t aTimeoutMs){
	if(!mIsBusOn){
		return(false);
	}
	return(mTxAckBuf.pop(aMsg, aTimeoutMs));
}

/* Interface implementation */
enum CanAdapter::CanAdapterState KvaserCanAdapter::getState(){
	if(!mIsOpen){
		return(CanAdapter::Closed);
	} else if(!mIsBusOn){
		return(CanAdapter::BusOff);
	} else {
		unsigned long busStatus;
		boost::mutex::scoped_lock lock(mHandleMutex);
		canStatus status = canReadStatus(mPortHandle, &busStatus);
		if(status != canOK){
			return(CanAdapter::Unknown);
		}
		if(busStatus & canSTAT_BUS_OFF){
			return(CanAdapter::BusOff);
		} else if (busStatus & canSTAT_ERROR_PASSIVE ){
			return(CanAdapter::ErrorPassive);
		} else if (busStatus & canSTAT_ERROR_ACTIVE ){
			return(CanAdapter::ErrorActive);
		} else {
			return(CanAdapter::Unknown);
		}
	}
}

/* Interface implementation */
void KvaserCanAdapter::getErrorCounters(int *aTxErrorCounter, int *aRxErrorCounter){
	if(!mIsBusOn){
		*aTxErrorCounter = -1;
		*aRxErrorCounter = -1;
	} else {
		unsigned int txErr, rxErr, ovErr;
		boost::mutex::scoped_lock lock(mHandleMutex);
		if(canReadErrorCounters(mPortHandle, &txErr, &rxErr, &ovErr) != canOK){
			*aTxErrorCounter = txErr;
			*aRxErrorCounter = rxErr;
		}
	}
}

void KvaserCanAdapter::receive(){
	canStatus status;
	long id;
	char msg[8];
	unsigned int dlc;
	unsigned int flag;
	unsigned long time;
	SharedCanMessage m;

	while(!mHaltReceiveThread){
		boost::mutex::scoped_lock lock(mHandleMutex);
		while( canRead(mPortHandle, &id, msg, &dlc, &flag, &time) == canOK){
			if(flag & canMSG_EXT){
				m = CanMessage::getSharedInstance(id, dlc, true);
			} else {
				m = CanMessage::getSharedInstance(id, dlc);
			}
			for(int i=0; i<dlc; i++){
				m->setData(i, msg[i]);
			}
			if(flag & canMSG_TXACK){
				mTxAckBuf.push(m, 0);
			} else {
				mRxBuf.push(m, 0);
			}
		}
		mCanEvent.wait(lock);
	}
}

void KvaserCanAdapter::notify(unsigned int notifyEvent){
	mCanEvent.notify_one();
}

void CANLIBAPI KvaserCanAdapter::notifyCallback(int hnd, void *classPtr, unsigned int notifyEvent){
	((KvaserCanAdapter *)classPtr)->notify(notifyEvent);
}
