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

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <boost/thread/thread.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/atomic.hpp>

#include "../utils/Logger.h"
#include "../utils/LogFile.h"

#include "SocketCanAdapter.h"

class SocketCanAdapter_p {
	static const int POLL_TIMEOUT_MS = 10;
	static const int DEFAULT_LINE_RX_TIMEOUT_MS = 3000;
	static const int CMD_TX_TIMEOUT_MS = 5000;
	enum {NumFilters = 1};

public:
	SocketCanAdapter_p(std::string aChannelName, uint32_t aBaudrate);
	virtual ~SocketCanAdapter_p();

	bool setParameter(std::string aKey, std::string aValue);
	bool setBaudRate(uint32_t aBaudrate);
	int getNumberOfFilters();
	bool setAcceptanceFilter(int fid, uint32_t code, uint32_t mask, bool isExt);

	bool open();
	void close();
	bool goBusOn();
	bool goBusOff();

	bool sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId);

	int numReceivedMessagesAvailable();
	int numSentMessagesAvailable();
	bool getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);
	bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);

private:
	bool toCanFrame(SharedCanMessage &aMsg, struct can_frame &aFrame);
	bool fromCanFrame(struct can_frame &aFrame, SharedCanMessage &aMsg);

	void doRead();
	void readEnd(struct can_frame &aRxFrame, const boost::system::error_code& error, size_t bytes_transferred);
	void doWrite();
	void writeEnd(struct can_frame &aTxFrame, const boost::system::error_code& error);
	void doClose();
	bool write(SharedCanMessage aMsg);

	boost::atomic_bool mIsOpen;
	CanMessageBuffer mRxBuf;
	CanMessageBuffer mTxBuf;
	CanMessageBuffer mTxAckBuf;

	std::string mChannelName;

	struct can_filter mFilter[NumFilters];
	uint32_t mBaudrate;

	boost::thread mThread; // for ioservice thread
	// only use in ioservice thread
	bool mWriteIsIdle;
	struct can_frame mRxFrame;
	struct can_frame mTxFrame;

	// socket stuff
	int mNatsock;
	boost::asio::io_service mIo; ///< Io service object
	boost::asio::posix::basic_stream_descriptor<> mStream;

	LogFile mLogFile;
};

SocketCanAdapter::SocketCanAdapter(std::string aChannelName, uint32_t aBaudrate):
			pimpl(new SocketCanAdapter_p(aChannelName, aBaudrate)){
}

SocketCanAdapter::~SocketCanAdapter(){
	pimpl.reset();
}

bool SocketCanAdapter::getFirstChannelName(std::string &aName){
	return false;
}

bool SocketCanAdapter::getNextChannelName(std::string &aName){
	return false;
}

bool SocketCanAdapter::setParameter(std::string aKey, std::string aValue){
	return pimpl->setParameter(aKey, aValue);
};

bool SocketCanAdapter::setBaudRate(uint32_t aBaudrate){
	return pimpl->setBaudRate(aBaudrate);
}

int SocketCanAdapter::getNumberOfFilters(){
	return pimpl->getNumberOfFilters();
}

bool SocketCanAdapter::setAcceptanceFilter(int fid, uint32_t code, uint32_t mask, bool isExt){
	return pimpl->setAcceptanceFilter(fid, code, mask, isExt);
}

bool SocketCanAdapter::open(){
	return pimpl->open();
}

bool SocketCanAdapter::goBusOn(){
	return pimpl->goBusOn();
}

bool SocketCanAdapter::goBusOff(){
	return pimpl->goBusOff();
}

bool SocketCanAdapter::sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId){
	return pimpl->sendMessage(aMsg, aTransactionId);
}

int SocketCanAdapter::numReceivedMessagesAvailable(){
	return pimpl->numReceivedMessagesAvailable();
}

bool SocketCanAdapter::getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	return pimpl->getReceivedMessage(aMsg, aTimeoutMs);
}

int SocketCanAdapter::numSentMessagesAvailable(){
	return pimpl->numSentMessagesAvailable();
}

bool SocketCanAdapter::getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	return pimpl->getSentMessage(aMsg, aTimeoutMs);
}

void SocketCanAdapter::close(){
	pimpl->close();
}

SocketCanAdapter_p::SocketCanAdapter_p(std::string aChannelName, uint32_t aBaudrate):
				mChannelName(aChannelName), mBaudrate(aBaudrate),
				mThread(), mRxBuf(), mTxBuf(), mTxAckBuf(), mIsOpen(false), mWriteIsIdle(false),
				mLogFile(), mIo(), mStream(mIo)
{
	for(int i=0; i<NumFilters; i++){
		mFilter[i].can_id   = 0;
		mFilter[i].can_mask = 0;
	}
}

SocketCanAdapter_p::~SocketCanAdapter_p(){
	if(mIsOpen){
		close();
	}
}

bool SocketCanAdapter_p::setParameter(std::string aKey, std::string aValue){
	if(aKey == "log_file"){
		mLogFile.setFileName(aValue);
		return true;
	} else {
		return false;
	}
};

void SocketCanAdapter_p::close(){
	if(mIsOpen){
		mIo.post(boost::bind(&SocketCanAdapter_p::doClose, this));
		mIo.reset();

		mTxBuf.clear();
		mRxBuf.clear();
		mTxAckBuf.clear();

		mLogFile.debugStream() << "closed" << std::endl;
		mLogFile.close();
	}
}

bool SocketCanAdapter_p::setBaudRate(uint32_t aBaudrate){
	mBaudrate = aBaudrate;
	return true;
}

int SocketCanAdapter_p::getNumberOfFilters(){
	return(NumFilters);
}

bool SocketCanAdapter_p::setAcceptanceFilter(int fid, uint32_t code, uint32_t mask, bool isExt){
	if(fid >= NumFilters){
		return false;
	}

	if(isExt){
		mFilter[fid].can_id = (code & CAN_EFF_MASK) | CAN_EFF_FLAG;
		mFilter[fid].can_mask = CAN_EFF_FLAG | CAN_RTR_FLAG | (mask & CAN_EFF_MASK);
	} else {
		mFilter[fid].can_id = (code & CAN_SFF_MASK);
		mFilter[fid].can_mask =  CAN_EFF_FLAG | CAN_RTR_FLAG | (mask & CAN_SFF_MASK);
	}
	return true;
}

bool SocketCanAdapter_p::open(){
	if(mIsOpen){
		return false;
	}

	struct sockaddr_can addr;
	struct ifreq ifr;

	mNatsock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if(mNatsock < 0){
		mLogFile.debugStream() << "Unable to create socket.";
		return false;
	}

	if(::setsockopt(
			mNatsock,
			SOL_CAN_RAW,
			CAN_RAW_FILTER,
			&mFilter,
			sizeof(mFilter)) < 0)
	{
		mLogFile.debugStream() << "Setting filter failed.";
		::close(mNatsock);
		return false;
	}

	strcpy(ifr.ifr_name, mChannelName.c_str());

	if(ioctl(mNatsock, SIOCGIFINDEX, &ifr) < 0){
		mLogFile.debugStream() << "ioctl() failed.";
		::close(mNatsock);
		return false;
	}

	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	if(bind(mNatsock,(struct sockaddr *)&addr,sizeof(addr)) < 0){
		mLogFile.debugStream() << "Unable to bind socket.";
		::close(mNatsock);
		return false;
	}

	mStream.assign(mNatsock);

	// this gives some work to the io_service before it is started
	// (without work, io_service::run will return)
	mIo.post(boost::bind(&SocketCanAdapter_p::doRead, this));

	mWriteIsIdle = true;
	boost::thread t(boost::bind(&boost::asio::io_service::run, &mIo));
	mThread.swap(t);

	mLogFile.open();

	mIsOpen = true;
	return true;
}


bool SocketCanAdapter_p::goBusOn(){
	if(!mIsOpen){
		return false;
	}
	return true;
}

bool SocketCanAdapter_p::goBusOff(){
	if(!mIsOpen){
		return false;
	}
	return true;
}

bool SocketCanAdapter_p::sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId){
	if(!mIsOpen){
		return false;
	}

	if(!write(aMsg)){
		return false;
	}

	// transaction ID not yet implememted
	aTransactionId = 0;
	// acknowledge transmit
	mTxAckBuf.push(CanMessage::getSharedInstance(aMsg), 0);
	return true;
}

int SocketCanAdapter_p::numReceivedMessagesAvailable(){
	if(!mIsOpen){
		return(0);
	}
	return mRxBuf.available();
}

bool SocketCanAdapter_p::getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	if(!mIsOpen){
		return false;
	}
	return mRxBuf.pop(aMsg, aTimeoutMs);
}

int SocketCanAdapter_p::numSentMessagesAvailable(){
	if(!mIsOpen){
		return(0);
	}
	return mTxAckBuf.available();
}

bool SocketCanAdapter_p::getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	if(!mIsOpen){
		return false;
	}
	return mTxAckBuf.pop(aMsg, aTimeoutMs);
}

bool SocketCanAdapter_p::write(SharedCanMessage aMsg){
	if(!mIsOpen){
		return false;
	}

	if(!mTxBuf.push(aMsg, 0)){
		return false;
	}
	// kick-off transmission (if not already going)
	mIo.post(boost::bind(&SocketCanAdapter_p::doWrite, this));
}

bool SocketCanAdapter_p::toCanFrame(SharedCanMessage &aMsg, struct can_frame &aFrame){
	aFrame.can_id  = aMsg->getId();
	if(aMsg->isExtended()){
		aFrame.can_id  |= CAN_EFF_FLAG;
	}

	aFrame.can_dlc = aMsg->getLen();
	for(int i=0; i<aMsg->getLen(); i++){
		aFrame.data[i] = aMsg->getData(i);
	}
	return true;
}

bool SocketCanAdapter_p::fromCanFrame(struct can_frame &aFrame, SharedCanMessage &aMsg){
	if((aFrame.can_id & CAN_RTR_FLAG) != 0){
		return false;
	} else if((aFrame.can_id & CAN_ERR_FLAG) != 0){
		return false;
	} else if((aFrame.can_id & CAN_EFF_FLAG) != 0){
		aMsg = CanMessage::getSharedInstance(aFrame.can_id & CAN_EFF_MASK, aFrame.can_dlc, true);
	} else {
		aMsg = CanMessage::getSharedInstance(aFrame.can_id & CAN_SFF_MASK, aFrame.can_dlc, false);
	}
	for(int i=0; i<aFrame.can_dlc; i++){
		aMsg->setData(i, aFrame.data[i]);
	}
	return true;
}

// this method is always executed in the ioservice thread
void SocketCanAdapter_p::doRead(){
	if(!mIsOpen){
		return;
	}
	mStream.async_read_some(boost::asio::buffer(&mRxFrame, sizeof(mRxFrame)),
			boost::bind(&SocketCanAdapter_p::readEnd, this ,
					boost::ref(mRxFrame),
		            boost::asio::placeholders::error,
		            boost::asio::placeholders::bytes_transferred));
}

// this method is always executed in the ioservice thread
void SocketCanAdapter_p::readEnd(struct can_frame& aRxFrame,
		const boost::system::error_code& error, size_t bytes_transferred){
	if(error){
		mLogFile.debugStream() << "Read end with error";
		if(mIsOpen){
			doClose();
		}
	} else {
		if(bytes_transferred == sizeof(mRxFrame)){
			SharedCanMessage m;
			if(fromCanFrame(aRxFrame, m)){
				mLogFile.debugStream() << "Read end: " << m;
				mRxBuf.push(m, 0);
			}
		}
		doRead();
	}
}

// this method is always executed in the ioservice thread
void SocketCanAdapter_p::doWrite(){
	if(!mIsOpen){
		return;
	}
	if(!mWriteIsIdle){
		// write already in progress
	}

	SharedCanMessage m;
	if(mTxBuf.pop(m, 0)){
		if(toCanFrame(m, mTxFrame)){
			mWriteIsIdle = false;
			mStream.async_write_some(boost::asio::buffer(&mTxFrame, sizeof(mTxFrame)),
					boost::bind(&SocketCanAdapter_p::writeEnd, this,
							boost::ref(mTxFrame),
							boost::asio::placeholders::error));
		}
	}
}

// this method is always executed in the ioservice thread
void SocketCanAdapter_p::writeEnd(struct can_frame &aTxFrame, const boost::system::error_code& error){
	if(error){
		mLogFile.debugStream() << "Write end with error";
		if(mIsOpen){
			doClose();
		}
	} else {
		SharedCanMessage m;
		if(fromCanFrame(aTxFrame, m)){
			mLogFile.debugStream() << "Write end: " << m;
			mTxAckBuf.push(m, 0);
		}
		mWriteIsIdle = true;
		doWrite();
	}
}

// this method is always executed in the ioservice thread
void SocketCanAdapter_p::doClose(){
	mIsOpen = false;
	mStream.cancel();
	mStream.close();
	::close(mNatsock); // TODO: make sure this is correct
}
