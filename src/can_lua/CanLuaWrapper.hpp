/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Copyright 2017 by CodeSkin LLC, www.codeskin.com.
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

#ifndef CAN_LUA_WRAPPER_HPP_
#define CAN_LUA_WRAPPER_HPP_

#ifdef SCONS_TARGET_WIN
#include <windows.h>
#endif
#ifdef SCONS_TARGET_UNIX
#include <dlfcn.h>
#endif

#include <stdio.h>
#include <vector>

#include "../utils/LibName.h"

#define C5_EMU_LUA_NAME LIB_NAME("CanLua")

class CanLuaWrapper_p;

class CanLuaWrapper {

public:
	CanLuaWrapper();
	virtual ~CanLuaWrapper();

	int loadDll();
	void unloadDll();

	int runScript(const char* aScriptName);
	int runScriptForHandle(const char* aScriptName, int aHandle);

private:
	class CanLuaWrapper_p *pimpl;
};

#ifdef SCONS_TARGET_WIN
inline static void CanLua_dispError(DWORD dw){
	LPVOID lpMsgBuf;

	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

	printf("Error: %s \n", lpMsgBuf);
}
#endif

class CanLuaWrapper_p {
	typedef int (*DllRunScriptFcn)(const char*);
	typedef int (*DllRunScriptForHandleFcn)(const char*, int);

public:
	inline CanLuaWrapper_p() {
		mHandle = NULL;
	}

	inline ~CanLuaWrapper_p(){
		unloadDll();
	}

#ifdef SCONS_TARGET_WIN
	inline void unloadDll(){
		if (mHandle)
		{
			FreeLibrary((HMODULE)mHandle);
			mHandle = NULL;
		}
	}

	inline int loadDll(){
		mHandle = LoadLibrary(C5_EMU_LUA_NAME);
		if (!mHandle){
			printf("Load of EMU Library failed with error: %d \n", GetLastError());
			CanLua_dispError(GetLastError());
			return(-1);
		} else {
			mDllRunScriptFcn = (DllRunScriptFcn)GetProcAddress((HMODULE)mHandle, "CANLUA_runScript");
			mDllRunScriptForHandleFcn = (DllRunScriptForHandleFcn)GetProcAddress((HMODULE)mHandle, "CANLUA_runScriptForHandle");
			if(!allPointersAreNonNull()){
				printf("Unable to obtain EMU function handles. Error: %d \n", GetLastError());
				CanLua_dispError(GetLastError());
				return(-2);
			}
		}
		return(0);
	}
#endif
#ifdef SCONS_TARGET_UNIX
	inline void unloadDll(){
		if (mHandle)
		{
			dlclose(mHandle);
			mHandle = NULL;
		}
	}

	inline int loadDll(){
		mHandle = dlopen(C5_EMU_LUA_NAME, RTLD_LAZY);
		if (!mHandle){
			printf("Load of EMU Library failed with error: %s \n", dlerror());
			return(-1);
		} else {
			mDllRunScriptFcn = (DllRunScriptFcn)dlsym(mHandle, "CanLua_runScript");
			mDllRunScriptForHandleFcn = (DllRunScriptForHandleFcn)dlsym(mHandle, "CanLua_runScriptForHandle");
			if(!allPointersAreNonNull()){
				printf("Unable to obtain EMU function handles. Error: %s \n", dlerror());
				return(-2);
			}
		}
		return(0);
	}
#endif

	inline DllRunScriptFcn getGetRunScriptFcn() const { return mDllRunScriptFcn; }
	inline DllRunScriptForHandleFcn getGetRunScriptForHandleFcn() const { return mDllRunScriptForHandleFcn; }

private:
	void* mHandle;

	DllRunScriptFcn mDllRunScriptFcn;
	DllRunScriptForHandleFcn mDllRunScriptForHandleFcn;

	inline int allPointersAreNonNull() {
		return(
				(mDllRunScriptFcn != NULL) &&
				(mDllRunScriptForHandleFcn != NULL)
		);
	}
};

inline CanLuaWrapper::CanLuaWrapper() {
	pimpl = new CanLuaWrapper_p();
}

inline CanLuaWrapper::~CanLuaWrapper(){
	delete pimpl;
}

inline int CanLuaWrapper::loadDll(){
	return pimpl->loadDll();
}

inline void CanLuaWrapper::unloadDll(){
	pimpl->unloadDll();
}

inline int CanLuaWrapper::runScript(const char* aScriptName){
	return pimpl->getGetRunScriptFcn()(aScriptName);
}

inline int CanLuaWrapper::runScriptForHandle(const char* aScriptName, int aHandle){
	return pimpl->getGetRunScriptForHandleFcn()(aScriptName, aHandle);
}

#endif /* CAN_LUA_WRAPPER_HPP_ */
