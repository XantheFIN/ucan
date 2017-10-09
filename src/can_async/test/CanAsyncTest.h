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

#ifndef CAN_ASYNC_TEST_H_
#define CAN_ASYNC_TEST_H_

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../CanAsyncIoObject.hpp"
#include "../CanAsyncService.hpp"

class CanAsyncTest {
public:
	CanAsyncTest(const boost::posix_time::time_duration aRxTimeout);
	virtual ~CanAsyncTest();

	bool start(SharedCanAdapter aCanAdapter);
	void stopAfterReceive();
	void stop();

private:
	void handleReceive(const boost::system::error_code &ec, SharedCanMessage aMsg);
	void handleSendEnd(const boost::system::error_code &ec, SharedCanMessage aMsg);
	void checkDeadline(const boost::system::error_code &ec);

	SharedCanMessage mTxMsg;
	int mRxCounter;
	boost::posix_time::ptime mStartTime;

	// order matters
	boost::asio::io_service mIo;
	CanAsyncIoObject<CanAsyncService<> > mClient;
	boost::asio::deadline_timer mTimer;
    boost::thread mBackgroundThread;
    bool mIsOpen;
    bool mCloseAfterRxReq;
    boost::posix_time::time_duration mRxTimeout;
};

#endif // CAN_ASYNC_TEST_H_
