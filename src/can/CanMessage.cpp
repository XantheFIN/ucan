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

#include "CanMessage.h"

namespace pt = boost::posix_time;

pt::ptime CanMessage::mCanEpoch = pt::microsec_clock::local_time();

SharedCanMessage CanMessage::getSharedInstance(uint32_t aId, unsigned int aLen, bool aIsExt){
	if(aId > 0x800){
		// automatically set extended ID if ID out of 11 bit range
		aIsExt = true;
	}
	SharedCanMessage msg(new CanMessage(aId, aLen,aIsExt));
	return msg;
}

SharedCanMessage CanMessage::getSharedInstance(SharedCanMessage aMsg){
	CanMessage *msg = new CanMessage(aMsg->getId(), aMsg->getLen(), aMsg->isExtended());
	for(int i=0; i<aMsg->getLen(); i++){
		msg->setData(i, aMsg->getData(i));
	}
	return(SharedCanMessage(msg));
}

CanMessage::CanMessage(uint32_t aId, unsigned int aLen, bool aIsExt) :
	mId(aId), mLen(aLen), mIsExt(aIsExt){

	pt::time_duration d = pt::microsec_clock::local_time() - mCanEpoch;
	mTimeStamp = d.total_milliseconds();

	for(int i=0; i<8; i++){
		mData[i] = DEFAULT_PADDING;
	}
}

CanMessage::CanMessage(){
}

CanMessage::~CanMessage(){
}

bool operator==(SharedCanMessage &m1, SharedCanMessage &m2){
	// TODO: perform comparison
	return(true);
}

bool operator!=(SharedCanMessage &m1, SharedCanMessage &m2){
	return(!(m1==m2));
}

std::ostream& operator<<(std::ostream& os, const SharedCanMessage& msg){
	if(msg == NULL){
		os << "NULL Msg";
	} else {
		os << "Msg ID: 0x" << std::hex << msg->mId << ", len: " << std::dec << msg->mLen;
	}
	return os;
}

