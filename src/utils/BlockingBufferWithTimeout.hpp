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

#pragma once

#include <stdint.h>

#include <queue>
#include <boost/thread/thread.hpp>

#pragma once

/**
 * Thread-safe buffer with timed-out blocking
 */

template<class M>
class BlockingBufferWithTimeout {
public:
	BlockingBufferWithTimeout(std::size_t aMaxEntries = 1024) :
			mBoostMutex(), mBoostNotifier(), mQueue(), mMaxEntries(aMaxEntries) {
	}

	~BlockingBufferWithTimeout(){};

	bool push(M msg,  uint32_t aTimeoutMs){
	    boost::mutex::scoped_lock lock(mBoostMutex);
	    if(mQueue.size() == mMaxEntries){
	    	return false;
	    }
	    mQueue.push(msg);
	    lock.unlock();
	    mBoostNotifier.notify_one();
	    return true;
	}

	bool pop(M &msg,  uint32_t aTimeoutMs){
	    boost::mutex::scoped_lock lock(mBoostMutex);
	    if(!mBoostNotifier.timed_wait(lock, boost::posix_time::milliseconds(aTimeoutMs),
	    		boost::bind(&BlockingBufferWithTimeout::isNotEmpty, this))){
	    	return false;
	    }

	    msg = mQueue.front();
	    mQueue.pop();
	    return true;
	}

	int32_t  available() const{
	    boost::mutex::scoped_lock lock(mBoostMutex);
	    return mQueue.size();
	}

	void clear(){
	    boost::mutex::scoped_lock lock(mBoostMutex);
	    std::queue<M> empty;
	    std::swap(mQueue, empty);
	}

private:
	bool isNotEmpty(){
		return !mQueue.empty();
	}

	mutable boost::mutex mBoostMutex;
	boost::condition_variable mBoostNotifier;
	std::size_t mMaxEntries;
	std::queue<M> mQueue;
};

