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

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/atomic.hpp>

#include "../utils/Logger.h"
#include "../utils/LogFile.h"
#include "../utils/SerialPortEnumerator.h"
#include "../utils/AsyncSerial.h"
#include "../utils/BlockingBufferWithTimeout.hpp"

#include "SLCanAdapter.h"

class SLCanAdapter_p {
	static const int POLL_TIMEOUT_MS = 10;
	static const int DEFAULT_LINE_RX_TIMEOUT_MS = 3000;
	static const int CMD_TX_TIMEOUT_MS = 5000;
	static const int StatusUpdateSpacingMs = 2000;
	static const int OpenPortTimeourtMs = 500;

public:
	SLCanAdapter_p(std::string aChannelName, uint32_t aBaudrate):
			mChannelName(aChannelName), mBaudrate(aBaudrate), mAc01(0), mAc23(0), mAm01(0), mAm23(0),
			mSerialBaudrate(115200),
			mThread(), mRxBuf(), mTxBuf(), mTxAckBuf(), mIsOpen(false),
			mLogFile(), mMutex(), mCmdMutex(), mReponseStatus(""), mNotifier(), mRxTimeout(DEFAULT_LINE_RX_TIMEOUT_MS),
			mLastStatusUpdateTime(boost::posix_time::neg_infin), mAdapterState(CanAdapter::Unknown)
	{
		;
	}

	~SLCanAdapter_p(){
		if(mIsOpen){
			close();
		}
	}

	static bool getFirstChannelName(std::string &aName);
	static bool getNextChannelName(std::string &aName);

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
	bool getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);
	int numSentMessagesAvailable();
	bool getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs);
	enum CanAdapter::CanAdapterState getState();

private:
	bool sendCommand(std::string aCommand, std::string &aReponse);
	bool getSetBaudrateCmd(uint32_t aBaudrate, std::string &aBaudrateCmd);
	void getSetFilterCmds(uint16_t aAc01, uint16_t aAc23, uint16_t aAm01, uint16_t aAm23, std::string &aCodeCmd, std::string &aMaskCmd);
	void encode(SharedCanMessage aMsg, std::string &aTxCmd);
	bool decode(std::string aRxCmd, SharedCanMessage &aMsg);
	void receive();
	void resetResponseStatus();
	bool responseStatusIsKnown();
	bool responseStatusIsOk();
	std::string getResponseStatus();
	void rxCallback(const char *data, unsigned int len);

	std::string mChannelName;
	uint32_t mBaudrate;
	uint16_t mAc01, mAc23;
	uint16_t mAm01, mAm23;

	static int mPortIndex;

	static std::vector<std::string> mDetectedPorts;
	boost::scoped_ptr<CallbackAsyncSerial> mAsyncSerial;
	BlockingBufferWithTimeout<char> mSerialRxBuf;

	uint32_t mSerialBaudrate;

	boost::thread mThread;

	boost::atomic_bool mIsOpen;
	CanMessageBuffer mRxBuf;
	CanMessageBuffer mTxBuf;
	CanMessageBuffer mTxAckBuf;

	std::vector<unsigned char> mInBuf;
	int mRxTimer;
	int mRxTimeout;

	std::string mReponseStatus;
	mutable boost::mutex mMutex;
	mutable boost::mutex mCmdMutex;
	boost::condition_variable mNotifier;

	boost::posix_time::ptime mLastStatusUpdateTime;
	CanAdapter::CanAdapterState mAdapterState;

	LogFile mLogFile;
};

const int SLCanAdapter_p::POLL_TIMEOUT_MS;
const int SLCanAdapter_p::CMD_TX_TIMEOUT_MS;
const int SLCanAdapter_p::DEFAULT_LINE_RX_TIMEOUT_MS;

int SLCanAdapter_p::mPortIndex = 0;
std::vector<std::string> SLCanAdapter_p::mDetectedPorts;

SLCanAdapter::SLCanAdapter(std::string aChannelName, uint32_t aBaudrate):
	pimpl(new SLCanAdapter_p(aChannelName, aBaudrate)){
}

SLCanAdapter::~SLCanAdapter(){
}

bool SLCanAdapter::getFirstChannelName(std::string &aName){
	return SLCanAdapter_p::getFirstChannelName(aName);
}

bool SLCanAdapter::getNextChannelName(std::string &aName){
	return SLCanAdapter_p::getNextChannelName(aName);
}

bool SLCanAdapter::setParameter(std::string aKey, std::string aValue){
	return pimpl->setParameter(aKey, aValue);
};

bool SLCanAdapter::setBaudRate(uint32_t aBaudrate){
	return pimpl->setBaudRate(aBaudrate);
}

int SLCanAdapter::getNumberOfFilters(){
	return pimpl->getNumberOfFilters();
}

bool SLCanAdapter::setAcceptanceFilter(int fid, uint32_t code, uint32_t mask, bool isExt){
	return pimpl->setAcceptanceFilter(fid, code, mask, isExt);
}

bool SLCanAdapter::open(){
	return pimpl->open();
}

bool SLCanAdapter::goBusOn(){
	return pimpl->goBusOn();
}

bool SLCanAdapter::goBusOff(){
	return pimpl->goBusOff();
}

bool SLCanAdapter::sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId){
	return pimpl->sendMessage(aMsg, aTransactionId);
}

int SLCanAdapter::numReceivedMessagesAvailable(){
	return pimpl->numReceivedMessagesAvailable();
}

bool SLCanAdapter::getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	return pimpl->getReceivedMessage(aMsg, aTimeoutMs);
}

int SLCanAdapter::numSentMessagesAvailable(){
	return pimpl->numSentMessagesAvailable();
}

bool SLCanAdapter::getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	return pimpl->getSentMessage(aMsg, aTimeoutMs);
}

enum CanAdapter::CanAdapterState SLCanAdapter::getState(){
	return pimpl->getState();
}

void SLCanAdapter::close(){
	pimpl->close();
}

bool SLCanAdapter_p::getFirstChannelName(std::string &aName){
	mPortIndex = 0;
	if(!SerialPortEnumerator::enumeratePorts(mDetectedPorts)){
		return false;
	}
	return(getNextChannelName(aName));
}

bool SLCanAdapter_p::getNextChannelName(std::string &aName){
	if(mPortIndex < 0){
		return false;
	}
	if(mPortIndex < mDetectedPorts.size()){
		aName = mDetectedPorts[mPortIndex];
		mPortIndex++;
		return true;
	}
	// no new channel found
	return(false);
}

bool SLCanAdapter_p::setParameter(std::string aKey, std::string aValue){
	try{
		if(aKey == "log_file"){
			mLogFile.setFileName(aValue);
			return true;
		} else if(aKey == "rx_timeout_ms"){
			int newRxTimeout = boost::lexical_cast<int>(aValue);
			if(newRxTimeout > 0){
				mRxTimeout = newRxTimeout;
				return true;
			}
		} else if(aKey == "serial_baudrate"){
			int mNewSerialBaudrate = boost::lexical_cast<int>(aValue);
			if(mNewSerialBaudrate > 0){
				mSerialBaudrate = mNewSerialBaudrate;
				return true;
			}
		}
	} catch (boost::bad_lexical_cast){
	}
	return false;
}

void SLCanAdapter_p::close(){
	if(mIsOpen){
		// close adapter
		std::string rsp;
		sendCommand("C", rsp);

		mIsOpen = false;
		mThread.join();

		mAsyncSerial->clearCallback();
		mAsyncSerial.reset();

		mTxBuf.clear();
		mRxBuf.clear();
		mTxAckBuf.clear();

		mLogFile.debugStream() << "closed" << std::endl;
		mLogFile.close();
	}
}

bool SLCanAdapter_p::setBaudRate(uint32_t aBaudrate){
	if(mIsOpen){
		// on-the-fly change not allowed
		return(false);
	}
	mBaudrate = aBaudrate;
	return true;
}

int SLCanAdapter_p::getNumberOfFilters(){
	return(2);
}

bool SLCanAdapter_p::setAcceptanceFilter(int fid, uint32_t code, uint32_t mask, bool isExt){
	if(mIsOpen){
		// on-the-fly change not allowed
		return(false);
	}

	/* masks need to be inverted (1=not relevant on SJA1000) */
	mask = (mask ^ 0x1FFFFFFF) & 0x1FFFFFFF;
	if(fid == 0){
		if(isExt){
			mAc01 = (code >> 13) & 0xFFFF;
			mAm01 = (mask >> 13) & 0xFFFF;
		} else {
			/*
			 * note that will mess-up an ext frame
			 * configuration of the other filter!
			 */
			mAc01 = (code << 5) & 0xFFFF;
			mAm01 = ((mask << 5) & 0xFFFF) | 0x1F;
			mAm23 = mAm23 | 0xF;
		}
		return(true);
	} else if (fid == 1){
		if(isExt){
			mAc23 = (code >> 13) & 0xFFFF;
			mAm23 = (mask >> 13) & 0xFFFF;
		} else {
			mAc23 = (code << 5) & 0xFFFF;
			mAm23 = (mask << 5) & 0xFFFF;
		}
		return(true);
	} else {
		return(false);
	}
}

bool SLCanAdapter_p::open(){
	if(mIsOpen){
		return false;
	}

	int timeElapsedMs = 0;
	int retryPauseMs = 20;
	while(true){
		try {
			mAsyncSerial.reset(new CallbackAsyncSerial(mChannelName.c_str(), mSerialBaudrate));
			break;
		} catch(boost::system::system_error& e) {
			if(timeElapsedMs < OpenPortTimeourtMs){
				// for some reason, close returns before port is actually released
				// this can create problems when trying to immediately reopen, and
				// so we try a couple of times
				timeElapsedMs += retryPauseMs;
				boost::this_thread::sleep_for(boost::chrono::milliseconds(retryPauseMs));
			} else {
				LOG(logERROR) << "Unable to open serial port";
				return false;
			}
		}
	}
	mAsyncSerial->setCallback(boost::bind(&SLCanAdapter_p::rxCallback,this,_1,_2));

	mLogFile.open();

	// start receive thread
	mIsOpen = true;
	boost::thread t(boost::bind(&SLCanAdapter_p::receive, this));
	mThread.swap(t);

	std::string rsp;

	// flush buffer
	bool success = false;
	for(int i=0; i<3; i++){
		if(sendCommand("", rsp)){
			success = true;
			break;
		}
	}

	// identify adapter
	if(success){
		if(!sendCommand("V", rsp)){
			success = false;
		} else {
			LOG(logDEBUG) << "Response to V command: " << rsp;
		}
	}
	if(success){
		if(!sendCommand("N", rsp)){
			success = false;
		} else {
			LOG(logDEBUG) << "Response to N command: " << rsp;
		}
	}

	if(success){
		// configure baudrate
		std::string baudrateCmd;
		if(!getSetBaudrateCmd(mBaudrate, baudrateCmd)){
			// invalid baudrate
			success = false;
		}
		if(success){
			if(!sendCommand(baudrateCmd, rsp)){
				success = false;
			}
		}
	}

	if(success){
		// configure filter
		std::string codeCmd;
		std::string maskCmd;
		SLCanAdapter_p::getSetFilterCmds(mAc01, mAc23, mAm01, mAm23, codeCmd, maskCmd);
		if(!sendCommand(codeCmd, rsp)){
			success = false;
		}
		if(success){
			if(!sendCommand(maskCmd, rsp)){
				success = false;
			}
		}
	}

	// open adapter
	if(success){
		if(!sendCommand("O", rsp)){
			success = false;
		}
	}

	if(!success){
		close();
		return false;
	}
	return true;
}

bool SLCanAdapter_p::goBusOn(){
	if(!mIsOpen){
		return false;
	}
	return true;
}

bool SLCanAdapter_p::goBusOff(){
	if(!mIsOpen){
		return false;
	}
	return true;
}

bool SLCanAdapter_p::sendMessage(SharedCanMessage aMsg, uint16_t *aTransactionId){
	if(!mIsOpen){
		return false;
	}

	std::string req, rsp;
	encode(aMsg, req);
	if(!sendCommand(req, rsp)){
		return false;
	}

	// transaction ID not yet implememted
	aTransactionId = 0;
	// acknowledge transmit
	mTxAckBuf.push(CanMessage::getSharedInstance(aMsg), 0);
	return true;
}

int SLCanAdapter_p::numReceivedMessagesAvailable(){
	if(!mIsOpen){
		return(0);
	}
	return(mRxBuf.available());
}

bool SLCanAdapter_p::getReceivedMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	if(!mIsOpen){
		return false;
	}
	return(mRxBuf.pop(aMsg, aTimeoutMs));
}

int SLCanAdapter_p::numSentMessagesAvailable(){
	if(!mIsOpen){
		return(0);
	}
	return(mTxAckBuf.available());
}

bool SLCanAdapter_p::getSentMessage(SharedCanMessage& aMsg, uint32_t aTimeoutMs){
	if(!mIsOpen){
		return false;
	}
	return(mTxAckBuf.pop(aMsg, aTimeoutMs));
}

enum CanAdapter::CanAdapterState SLCanAdapter_p::getState(){
	if(!mIsOpen){
		mAdapterState = CanAdapter::Closed;
	} else {
		mAdapterState = CanAdapter::Unknown;
		boost::posix_time::time_duration d = boost::posix_time::microsec_clock::local_time() - mLastStatusUpdateTime;
		if(d.total_milliseconds() > StatusUpdateSpacingMs){
			mLastStatusUpdateTime = boost::posix_time::microsec_clock::local_time();
			std::string rsp;
			if(!sendCommand("F", rsp)){
				mAdapterState = CanAdapter::Unknown;
			} else {
				if(!boost::algorithm::starts_with(rsp, "F")){
					mAdapterState = CanAdapter::Unknown;
				} else {
					int status = (int)strtol(rsp.substr(1,2).c_str(), NULL, 16);
					//	Bit 0 CAN receive FIFO queue full
					//	Bit 1 CAN transmit FIFO queue full
					//	Bit 2 Error warning (EI), see SJA1000 datasheet
					//	Bit 3 Data Overrun (DOI), see SJA1000 datasheet
					//	Bit 4 Not used.
					//	Bit 5 Error Passive (EPI), see SJA1000 datasheet
					//	Bit 6 Arbitration Lost (ALI), see SJA1000 datasheet *
					//	Bit 7 Bus Error (BEI), see SJA1000 datasheet **
					if((status & 0x80) != 0){
						mAdapterState = CanAdapter::BusOff;
					} else if((status & 0x20) != 0){
						mAdapterState = CanAdapter::ErrorPassive;
					}
				}
			}
		}
	}
	return(mAdapterState);
}

bool SLCanAdapter_p::getSetBaudrateCmd(uint32_t aBaudrate, std::string &aBaudrateCmd){
	if(aBaudrate == 0){
		return false;
	}

	if((aBaudrate & 0x80000000) ==  0x80000000){
		// special "NI" convention
		std::ostringstream oss;
		oss << "s" <<  std::hex << std::setw(2) << ((aBaudrate & 0xFF) << 8) << std::setw(2) << ((aBaudrate & 0xFF00) >> 8);
		aBaudrateCmd =  oss.str();
	} else {
		switch(aBaudrate){
			case 10000:
				aBaudrateCmd = "S0";
				break;
			case 20000:
				aBaudrateCmd = "S1";
				break;
			case 50000:
				aBaudrateCmd = "S2";
				break;
			case 100000:
				aBaudrateCmd = "S3";
				break;
			case 125000:
				aBaudrateCmd = "S4";
				break;
			case 250000:
				aBaudrateCmd = "S5";
				break;
			case 500000:
				aBaudrateCmd = "S6";
				break;
			case 800000:
				aBaudrateCmd = "S7";
				break;
			case 1000000:
				aBaudrateCmd = "S8";
				break;
			default:
				return false;
		}
	}
	return true;
}

void SLCanAdapter_p::getSetFilterCmds(uint16_t aAc01, uint16_t aAc23, uint16_t aAm01, uint16_t aAm23,
		std::string &aCodeCmd, std::string &aMaskCmd){

	std::ostringstream osc;
	osc << "M" <<  std::hex << std::setfill('0') << std::setw(4) << aAc01 <<  std::setw(4) << aAc23;
	std::ostringstream osm;
	osm << "m" <<  std::hex << std::setfill('0') << std::setw(4) << aAm01 <<  std::setw(4) << aAm23;
	aCodeCmd = osc.str();
	aMaskCmd = osm.str();
}

void SLCanAdapter_p::encode(SharedCanMessage aMsg, std::string &aTxCmd){
	std::ostringstream oss;
	if(aMsg->isExtended()){
		oss << "T" << std::setfill('0') << std::hex << std::setw(8) << aMsg->getId();
	} else {
		oss << "t" << std::setfill('0') << std::hex << std::setw(3) << aMsg->getId();
	}
	oss << std::setw(1) << aMsg->getLen();
	for(int i=0; i<aMsg->getLen(); i++){
		oss << std::setw(2) << (int)aMsg->getData(i);
	}
	aTxCmd = oss.str();
}

bool SLCanAdapter_p::decode(std::string aRxCmd, SharedCanMessage &aMsg){
	SharedCanMessage m;
	int dataStartIndex;
	try {
		if(boost::algorithm::starts_with(aRxCmd, "T")){
			int id = (int)strtol(aRxCmd.substr(1,8).c_str(), NULL, 16);
			int dlc = (int)strtol(aRxCmd.substr(9,1).c_str(), NULL, 16);
			m = CanMessage::getSharedInstance(id, dlc, true);
			dataStartIndex = 10;
		} else if(boost::algorithm::starts_with(aRxCmd, "t")){
			int id = (int)strtol(aRxCmd.substr(1,3).c_str(), NULL, 16);
			int dlc = (int)strtol(aRxCmd.substr(4,1).c_str(), NULL, 16);
			m = CanMessage::getSharedInstance(id, dlc);
			dataStartIndex = 5;
		} else {
			return false;
		}
		for(int i=0; i<m->getLen(); i++){
			int b = (int)strtol(aRxCmd.substr(dataStartIndex,2).c_str(), NULL, 16);
			m->setData(i, b);
			dataStartIndex += 2;
		}
	} catch(...) {
		return false;
	}
	aMsg = m;
	return true;
}

void SLCanAdapter_p::resetResponseStatus(){
	mReponseStatus = "?";
}

bool SLCanAdapter_p::responseStatusIsKnown(){
	return(mReponseStatus != "?");
}

bool SLCanAdapter_p::responseStatusIsOk(){
	return responseStatusIsKnown() && (mReponseStatus != "!");
}

std::string SLCanAdapter_p::getResponseStatus(){
	return mReponseStatus;
}

bool SLCanAdapter_p::sendCommand(std::string aCommand, std::string &aResponse){
	boost::mutex::scoped_lock cmdLock(mCmdMutex);

	mLogFile.debugStream() << "cmd: " << aCommand;

	// terminate with CR
	if(!boost::algorithm::ends_with(aCommand, "\r")){
		aCommand += "\r";
	}
	// convert to char vector and
	std::vector<char> vCmd(aCommand.begin(), aCommand.end());

	boost::mutex::scoped_lock lock(mMutex);
	resetResponseStatus();

	mAsyncSerial->write(vCmd);

	mNotifier.timed_wait(lock, boost::posix_time::milliseconds(CMD_TX_TIMEOUT_MS),
			boost::bind(&SLCanAdapter_p::responseStatusIsKnown, this));

	if(!responseStatusIsKnown()){
		// no response!
		mLogFile.debugStream() << "rsp: timeout";
		return false;
	}

	if(!responseStatusIsOk()){
		mLogFile.debugStream() << "rsp: error";
		return false;
	}

	aResponse = getResponseStatus();
	mLogFile.debugStream() << "rsp: " << aResponse;
	return true;
}

void SLCanAdapter_p::receive(){
	char inc;

	bool eol = false;
	bool err = false;
	while(!mIsOpen){
		if(mSerialRxBuf.pop(inc, POLL_TIMEOUT_MS)){
			mRxTimer = 0;
			if(inc == '\r'){
				eol = true;
			} else if(inc == 0x07){
				err = true;
			} else {
				mInBuf.push_back(inc);
			}
		} else {
			// nothing received
			if(mInBuf.size() > 0){
				// we have data lingering...
				mRxTimer += POLL_TIMEOUT_MS;
				if(mRxTimer > mRxTimeout){
					mLogFile.debugStream() << "Rx timeout";
					mInBuf.clear();
				}
			} else {
				mRxTimer = 0;
			}
		}

		if(err){
			{
				boost::mutex::scoped_lock lock(mMutex);
				mReponseStatus = "!";
			}
			mNotifier.notify_one();
			mInBuf.clear();
			eol = false;
			err = false;
		} else if(eol){
			std::string strMsg(mInBuf.begin(), mInBuf.end());
			if(boost::algorithm::starts_with(strMsg, "t") || boost::algorithm::starts_with(strMsg, "T")){
				// message received
				SharedCanMessage m;
				if(decode(strMsg, m)){
					mRxBuf.push(m, 0);
				}
			} else {
				// command response
				{
					boost::mutex::scoped_lock lock(mMutex);
					mReponseStatus = strMsg;
				}
				mNotifier.notify_one();
			}
			mInBuf.clear();
			eol = false;
			err = false;
		}
	}
}

void SLCanAdapter_p::rxCallback(const char *data, unsigned int len){
	for(int i=0; i<len; i++){
		if(!mSerialRxBuf.push(data[i], 0)){
			break; // buffer overrun
		}
	}
}
