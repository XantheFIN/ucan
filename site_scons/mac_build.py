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

from SCons.Util import *
import os
import sys

def set_build_options(env):
	env.Append(CCFLAGS = ['-DSCONS_TARGET_UNIX'])
	env.Append(CCFLAGS = ['-DSCONS_TARGET_MAC'])   
	
	if int(env['ARCH']) == 32:
		raise Exception, '32-bit architecture not supported.'
		
	tool_path = [
		'/opt/local/bin/',
    	'/usr/local/lib',
    	'/usr/local/include'
    	];
    
    #augment search path
	env.PrependENVPath('PATH', tool_path)
	
	inc_search_path = [
		'/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers',
		'/usr/local/Cellar/boost/1.63.0/include'
		];
		
	env.Append(LIBPATH = ['/usr/local/Cellar/boost/1.63.0/lib']);	
	
	#augment include path
	env.Append(CPPPATH = inc_search_path)
	
	env.Append(CCFLAGS = ['-Wno-deprecated-declarations'])
    
 	return env
