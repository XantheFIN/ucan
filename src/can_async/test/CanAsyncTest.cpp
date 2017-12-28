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

#include <iostream>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <boost/system/error_code.hpp>

#include "CanAsyncTest.h"
#include "../../utils/Logger.h"

CanAsyncTest::CanAsyncTest(const boost::posix_time::time_duration aRxTimeout) :
	mIo(), mClient(mIo), mTimer(mIo),
	mBackgroundThread(), mIsOpen(false), mCloseAfterRxReq(false), mRxTimeout(aRxTimeout){
}

CanAsyncTest::~CanAsyncTest(){
}

bool CanAsyncTest::start(SharedCanAdapter CanAdapter){
	if(!mClient.open(CanAdapter)){
		return false;
	}
	mIsOpen = true;
	mCloseAfterRxReq = false;
	mClient.asyncGetSendAcknMessage(boost::bind(&CanAsyncTest::handleSendEnd, this, _1, _2));
	mClient.asyncGetReceivedMessage(boost::bind(&CanAsyncTest::handleReceive, this, _1, _2));
	boost::thread t(boost::bind(&boost::asio::io_service::run, &mIo));
	mBackgroundThread.swap(t);

	mTxMsg = CanMessage::getSharedInstance(0x770, 8);
	mTxMsg->setData(0, 0x02);
	mTxMsg->setData(1, 0x3E);
	mTxMsg->setData(2, 0x00);
	for(int i=3; i<7; i++){
		mTxMsg->setData(i, 0xFF);
	}

	mRxCounter = 0;
	mStartTime = boost::posix_time::microsec_clock::local_time();

	mTimer.expires_from_now(mRxTimeout);
	mTimer.async_wait(boost::bind(&CanAsyncTest::checkDeadline, this, _1));
	mClient.sendMessage(mTxMsg);
	return mIsOpen;
}

void CanAsyncTest::stopAfterReceive(){
	if(!mIsOpen){
		return;
	}
	mCloseAfterRxReq = true;
	mBackgroundThread.join();
	mIo.reset();
	mIsOpen = false;
}

void CanAsyncTest::stop(){
	if(!mIsOpen){
		return;
	}
	mTimer.cancel();
	mClient.close();
	mIo.stop();
	mBackgroundThread.join();
	mIo.reset();
	mIsOpen = false;
}

void CanAsyncTest::handleSendEnd(const boost::system::error_code &ec, SharedCanMessage aMsg){
	if(ec){
		// most likely this means that operation has been cancelled
	} else {
	    std::cout << "CAN msg sent: " << aMsg << " - @" << aMsg->getTimeStamp() << std::endl << std::flush;;
	    mClient.asyncGetSendAcknMessage(boost::bind(&CanAsyncTest::handleSendEnd, this, _1, _2));
	}
}

void CanAsyncTest::handleReceive(const boost::system::error_code &ec, SharedCanMessage aMsg){
	if(ec){
		// most likely this means that operation has been cancelled
	} else {
		if(mTimer.expires_at(boost::posix_time::pos_infin) > 0){
	    	// timer was cancelled/re-scheduled in time
	    	mTimer.async_wait(boost::bind(&CanAsyncTest::checkDeadline, this, _1));
		} else {
			// too late - timer event handler already queued...
		}
		mRxCounter++;
		if(mIsOpen){
			if(mRxCounter < 10){
			    boost::posix_time::ptime mst2 = boost::posix_time::microsec_clock::local_time();
			    boost::posix_time::time_duration msdiff = mst2 - mStartTime;
			    mStartTime = mst2;
			    std::cout << "CAN msg received: " << aMsg << " - @" << aMsg->getTimeStamp() << " - Time elapsed [ms]: " << (msdiff.total_milliseconds()) << std::endl << std::flush;;
			    if(mTimer.expires_from_now(mRxTimeout) > 0){
			    	// timer was cancelled/re-scheduled in time
			    	mTimer.async_wait(boost::bind(&CanAsyncTest::checkDeadline, this, _1));
			    } else {
					// too late - timer event handler already queued...
			    }
			    mClient.sendMessage(mTxMsg);
			    mClient.asyncGetReceivedMessage(boost::bind(&CanAsyncTest::handleReceive, this, _1, _2));
			} else if (!mCloseAfterRxReq){
				mClient.asyncGetReceivedMessage(boost::bind(&CanAsyncTest::handleReceive, this, _1, _2));
			} else {
				mClient.close();
				mTimer.cancel();
			}
		}
	}
}

void CanAsyncTest::checkDeadline(const boost::system::error_code &ec){
	if(ec){
		// timer has been cancelled (or rescheduled before expiration()
	} else if (mIsOpen){
		// timer has expired - check if it has been rescheduled since
		if (mTimer.expires_at() > boost::asio::deadline_timer::traits_type::now()){
			// we have been rescheduled, so keep waiting
			mTimer.async_wait(boost::bind(&CanAsyncTest::checkDeadline, this, _1));
		} else {
			// this is a valid timeout
			mClient.close();
		}
	}
}

