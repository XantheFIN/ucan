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

#ifdef SCONS_TARGET_UNIX

#include <dlfcn.h>

void __attribute__ ((constructor)) CanDll_load(void);
void __attribute__ ((destructor)) CanDll_unload(void);

extern void CAN_load(const char *dllNamePath);
extern void CAN_unload(void);

// Called when the library is loaded and before dlopen() returns
void CanDll_load(void){
    Dl_info info;
    if (!dladdr((const void*)CAN_load, &info)) {
    	CAN_load(const_cast<char*>(""));
    } else {
    	CAN_load(info.dli_fname);
    }

}

// Called when the library is unloaded and before dlclose() returns
void CanDll_unload(void){
	CAN_unload();
}

#endif //SCONS_TARGET_UNIX
