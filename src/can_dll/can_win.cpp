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

#ifdef SCONS_TARGET_WIN

#include <stdio.h>
#include <windows.h>
#include <winbase.h>

#include "../utils/Logger.h"

extern void CAN_load(const char *dllNamePath);
extern void CAN_unload(void);

BOOL WINAPI DllMain(
		HINSTANCE hinstDLL,  // handle to DLL module
		DWORD fdwReason,     // reason for calling function
		LPVOID lpReserved ){  // reserved
	// Perform actions based on the reason for calling.
	switch( fdwReason )
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		// >>> Called when library is loaded

		char dllPath[MAX_PATH];
		GetModuleFileName(hinstDLL, dllPath, MAX_PATH);

		CAN_load(dllPath);
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		// >>> Called when library unloaded
		CAN_unload();
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#endif
