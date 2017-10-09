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

#include <stddef.h>
#include <stdio.h>
#include <iostream>

#ifdef SCONS_TARGET_WIN
#include <windows.h>
#endif
#ifdef SCONS_TARGET_UNIX
#include <dlfcn.h>
#endif

#include "CanAdapterWrapper.h"

CanAdapterWrapper::CanAdapterWrapper(std::string aDllName):
	mHandle(NULL),
	mDllName(aDllName){

	setDefaultPointers();
}

void CanAdapterWrapper::setDefaultPointers(){
	mCreateCanAdapterInstanceFcn = (DllCreateCanAdapterInstanceFcn)defaultCreateCanAdapterInstance;
	mGetFirstChannelNameFcn = (DllGetFirstChannelNameFcn)defaultGetFirstChannelName;
	mGetNextChannelNameFcn = (DllGetNextChannelNameFcn)defaultGetNextChannelName;
}

CanAdapterWrapper::~CanAdapterWrapper(){
	unload();
}

void CanAdapterWrapper::unload(){
	if (mHandle)
	{
#ifdef SCONS_TARGET_WIN
		FreeLibrary((HMODULE)mHandle);
#endif
#ifdef SCONS_TARGET_UNIX
    	dlclose(mHandle);
#endif
		mHandle = NULL;
		setDefaultPointers();
	}
}

bool CanAdapterWrapper::load(){
#ifdef SCONS_TARGET_WIN
	mHandle = LoadLibrary(mDllName.c_str());
#endif
#ifdef SCONS_TARGET_UNIX
	mHandle = dlopen(mDllName.c_str(), RTLD_LAZY);
#endif
	if (!mHandle){
		std::cout << "Unable to load adapter DLL: " << mDllName << std::endl;
#ifdef SCONS_TARGET_UNIX
		printf("Error: %s \n", dlerror());
#endif
		setDefaultPointers();
		return(false);
	} else {
#ifdef SCONS_TARGET_WIN
		mCreateCanAdapterInstanceFcn = (DllCreateCanAdapterInstanceFcn)GetProcAddress((HMODULE)mHandle, "createCanAdapterInstance");
		mGetFirstChannelNameFcn = (DllGetFirstChannelNameFcn)GetProcAddress((HMODULE)mHandle, "getFirstChannelName");
		mGetNextChannelNameFcn = (DllGetNextChannelNameFcn)GetProcAddress((HMODULE)mHandle, "getNextChannelName");
#endif
#ifdef SCONS_TARGET_UNIX
		mCreateCanAdapterInstanceFcn = (DllCreateCanAdapterInstanceFcn)dlsym(mHandle, "createCanAdapterInstance");
		mGetFirstChannelNameFcn = (DllGetFirstChannelNameFcn)dlsym(mHandle, "getFirstChannelName");
		mGetNextChannelNameFcn = (DllGetNextChannelNameFcn)dlsym(mHandle, "getNextChannelName");
#endif
		if(!allPointersAreNonNull()){
			std::cout << "Unable to obtain function pointers for adapter DLL: " << mDllName << std::endl;
			setDefaultPointers();
			return(false);
		}
	}
	std::cout << "Loaded adapter DLL: " << mDllName << std::endl;
	return(true);
}

bool CanAdapterWrapper::allPointersAreNonNull() {
	return(
			(mCreateCanAdapterInstanceFcn != NULL) &&
			(mGetFirstChannelNameFcn != NULL) &&
			( mGetNextChannelNameFcn != NULL)
	);
}

