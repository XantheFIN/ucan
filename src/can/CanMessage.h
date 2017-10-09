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

#ifndef CAN_MESSAGE_H_
#define CAN_MESSAGE_H_

#ifdef SCONS_TARGET_WIN
#include <stdint.h>
#endif

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class CanMessage;
typedef boost::shared_ptr<CanMessage> SharedCanMessage;

class CanMessage{
public:
	static const uint8_t DEFAULT_PADDING = 0xFF;

	static SharedCanMessage getSharedInstance(uint32_t aId, unsigned int aLen, bool aIsExt=false);
	static SharedCanMessage getSharedInstance(SharedCanMessage aMsg);
	virtual ~CanMessage();

	uint32_t getId() const {return mId; };
	unsigned int getLen() const {return mLen; };
	bool isExtended() const {return mIsExt; };
	uint32_t getTimestamp() const {return mTimeStamp; };
	uint8_t getData(unsigned int aIndex) const {return mData[aIndex]; };
	void setData(unsigned int aIndex, uint8_t aData) { mData[aIndex] = aData; };
	uint32_t getTimeStamp() const {return mTimeStamp; };

	// for display of messages
	friend std::ostream& operator<<(std::ostream& os, const SharedCanMessage& msg);

	// for testing and debugging
	friend bool operator== (SharedCanMessage &m1, SharedCanMessage &m2);
	friend bool operator!= (SharedCanMessage &m1, SharedCanMessage &m2);

private:
	CanMessage();
	CanMessage(uint32_t aId, unsigned int aLen, bool aIsExt);

	// make uncopyable (we only want to use shared pointers
	CanMessage(const CanMessage&);
	CanMessage& operator=(const CanMessage&);

	static boost::posix_time::ptime mCanEpoch;

	uint32_t mId;
	bool mIsExt; // true if extended message
	unsigned int mLen; // message length
	uint8_t mData[8]; // data (with padding if necessary)
	uint32_t mTimeStamp; // time-stamp of when message was created (received, tx ackn, etc)
};

#endif // CAN_MESSAGE_H_
