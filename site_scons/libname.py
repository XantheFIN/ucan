"""
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
"""

# Defines DLL naming

import os
import platform

def lib_name(env, libname):
	if((os.name == 'posix') and (platform.system() == 'Darwin' )):
		return libname
	elif((os.name == 'posix') and (platform.system() == 'Linux' )):
		return libname
	elif ((os.name == 'nt') and (platform.system() == 'Windows' )):
		if int(env['ARCH']) == 32:
			return libname+"_32"
		else:
			return libname+"_64"

def sharedlib_name(env, libname):
	if((os.name == 'posix') and (platform.system() == 'Darwin' )):
		return "libC5"+libname
	elif((os.name == 'posix') and (platform.system() == 'Linux' )):
		return "libC5"+libname
	elif ((os.name == 'nt') and (platform.system() == 'Windows' )):
		if int(env['ARCH']) == 32:
			return "C5"+libname+"-md_32"
		else:
			return "C5"+libname+"-md_64"
							
def sharedlib_name_with_ext(env, libname):
	if((os.name == 'posix') and (platform.system() == 'Darwin' )):
		return sharedlib_name(env, libname)+".dylib"
	elif((os.name == 'posix') and (platform.system() == 'Linux' )):
		return sharedlib_name(env, libname)+".so"
	elif ((os.name == 'nt') and (platform.system() == 'Windows' )):
		return sharedlib_name(env, libname)+".dll"
		
def external_sharedlib_name_with_ext(env, libname):
	if((os.name == 'posix') and (platform.system() == 'Darwin' )):
		return '#extern/mac/dll/'+sharedlib_name_with_ext(env, libname)
	elif((os.name == 'posix') and (platform.system() == 'Linux' )):
		return '#extern/linux/dll/'+sharedlib_name_with_ext(env, libname)
	elif ((os.name == 'nt') and (platform.system() == 'Windows' )):
		return '#extern/win/dll/'+sharedlib_name_with_ext(env, libname)	
		

