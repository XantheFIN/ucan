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
#include <vector>


template<class M>
class HandleManager {
public:
	HandleManager(M aNullHandle, std::size_t aMaxHandles = 1024) :
		mInstances(), mRecycledHandles(), mNextFreshHandle(1), mMaxHandles(aMaxHandles) {
		mInstances.push_back(aNullHandle);
	}

	~HandleManager(){};

	std::size_t assignToHandle(M aInstance){
		std::size_t handle = 0;
		if(mRecycledHandles.size() != 0){
			handle = mRecycledHandles.front();
			mRecycledHandles.pop();
			mInstances[handle] = aInstance;
		} else {
			if(mNextFreshHandle < mMaxHandles){
				handle = mNextFreshHandle;
				mNextFreshHandle++;
				mInstances.push_back(aInstance);
			}
		}
		return handle;
	}

	void releaseHandle(std::size_t aHandle){
		if((aHandle+1) == mNextFreshHandle){
			mInstances.pop_back();
			mNextFreshHandle--;
		} else {
			mInstances[aHandle] = mInstances[0];
			mRecycledHandles.push(aHandle);
		}
	}

	void releaseAllHandles(){
		mInstances.clear();
		std::queue<std::size_t> empty;
		std::swap(mRecycledHandles, empty);
		mNextFreshHandle = 0;
	}

	M& operator [](std::size_t aHandle){
		if(aHandle < mNextFreshHandle){
			return mInstances[aHandle];
		} else {
			return mInstances[0];
		}
	}

	bool isValid(std::size_t aHandle){
		if(aHandle < mNextFreshHandle){
			return (mInstances[aHandle] != mInstances[0]);
		} else {
			return false;
		}
	}

	std::size_t nextFreshHandle(){
		return mNextFreshHandle;
	}

private:
	std::size_t mNextFreshHandle;
	std::size_t mMaxHandles;
	std::vector<M> mInstances;
	std::queue<std::size_t> mRecycledHandles;
};

