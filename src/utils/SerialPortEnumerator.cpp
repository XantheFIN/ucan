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

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "SerialPortEnumerator.h"

#ifdef SCONS_TARGET_WIN
#pragma comment(lib, "advapi32.lib")
#include "Windows.h"
#endif

#ifdef SCONS_TARGET_MAC
#include <sys/param.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#endif

#ifdef SCONS_TARGET_LINUX
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
namespace fs = boost::filesystem;
#endif

SerialPortEnumerator::SerialPortEnumerator(){
}

SerialPortEnumerator::~SerialPortEnumerator(){
}

bool SerialPortEnumerator::enumeratePorts(std::vector<std::string>& ports){
	ports.clear();
#if defined(SCONS_TARGET_WIN)
	bool success = false;
	HKEY keyHandle;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &keyHandle) == ERROR_SUCCESS){
		DWORD maxValueNameLen, maxValueLen;
		LONG result;
		result = RegQueryInfoKey(keyHandle, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &maxValueNameLen, &maxValueLen, NULL, NULL);
		if (result == ERROR_SUCCESS){
			char *valueName = (char *)malloc(sizeof(char)*(maxValueNameLen+1));
			char *value = (char *)malloc(sizeof(char)*(maxValueLen+1));
			if((valueName != NULL) && (value != NULL)){
				DWORD index = 0;
				while (true){
					DWORD type;
					DWORD valueNameLen = maxValueNameLen+1;
					DWORD valueLen = maxValueLen+1;
					result = RegEnumValue(keyHandle, index, valueName, &valueNameLen, NULL, &type, (LPBYTE)value, &valueLen);
					if(result != ERROR_SUCCESS){
						break;
					}
					if(type == REG_SZ){
						value[valueLen] = '\0';
						std::string valueS(value);
						ports.push_back(valueS);
					}
					index++;
				}
				free(valueName);
				free(value);
				success = true;
			}
		}
		RegCloseKey(keyHandle);
	}
	return success;
#elif defined(SCONS_TARGET_MAC)
	// Serial devices are instances of class IOSerialBSDClient.
	// Create a matching dictionary to find those instances.
	CFMutableDictionaryRef classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
	if (classesToMatch == NULL) {
		printf("IOServiceMatching returned a NULL dictionary.\n");
	}
	else {
		// Look for devices that claim to be modems.
		CFDictionarySetValue(classesToMatch,
				CFSTR(kIOSerialBSDTypeKey),
				CFSTR(/*kIOSerialBSDModemType*/kIOSerialBSDRS232Type));

		// Each serial device object has a property with key
		// kIOSerialBSDTypeKey and a value that is one of kIOSerialBSDAllTypes,
		// kIOSerialBSDModemType, or kIOSerialBSDRS232Type. You can experiment with the
		// matching by changing the last parameter in the above call to CFDictionarySetValue.

		// As shipped, this sample is only interested in modems,
		// so add this property to the CFDictionary we're matching on.
		// This will find devices that advertise themselves as modems,
		// such as built-in and USB modems. However, this match won't find serial modems.
	}

	// Get an iterator across all matching devices.
	io_iterator_t matchingServices;
	kern_return_t kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, &matchingServices);
	if (KERN_SUCCESS != kernResult) {
		printf("IOServiceGetMatchingServices returned %d\n", kernResult);
		return false;
	}

	io_object_t	modemService;
	while ((modemService = IOIteratorNext(matchingServices))) {
		CFTypeRef	bsdPathAsCFString;

		// Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
		// used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
		// incoming calls, e.g. a fax listener.
		bsdPathAsCFString = IORegistryEntryCreateCFProperty(modemService,
				CFSTR(kIOCalloutDeviceKey),
				kCFAllocatorDefault,
				0);
		if (bsdPathAsCFString) {
			char bsdPath[MAXPATHLEN];
			*bsdPath = '\0';

			// Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
			// used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
			// incoming calls, e.g. a fax listener.
			bsdPathAsCFString = IORegistryEntryCreateCFProperty(modemService,
					CFSTR(kIOCalloutDeviceKey),
					kCFAllocatorDefault,
					0);
			Boolean result = CFStringGetCString((CFStringRef)bsdPathAsCFString,
					(char *)&bsdPath,
					sizeof(bsdPath),
					kCFStringEncodingUTF8);
			CFRelease(bsdPathAsCFString);

			std::string spath(bsdPath);
			ports.push_back(spath);
		}
	}
	return true;
#elif defined(SCONS_TARGET_LINUX)
	/*
	 * Notes:
	 * - internal interfaces are not detected
	 * - /dev/serial only exists if interfaces present
	 */
	fs::path p("/dev/serial/by-id");
	try {
		fs::directory_iterator it(p), eod;
		BOOST_FOREACH(fs::path const &x, std::make_pair(it, eod))
		{
			//	td::string spath(fs::canonical(x).string());
			std::string spath(x.string());
			ports.push_back(spath);
		}
	} catch (const fs::filesystem_error& ex) {
		return false;
	}
	return true;
#else
	return false;
#endif
}
