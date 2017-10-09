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

# NOTE r prefixing a string disables the \ escape characters
def set_build_options(env):
    env.Append(CCFLAGS = ['-DSCONS_TARGET_WIN'])
    
    #toolchain directories
    VS_PATH = 'C:/Program Files (x86)/Microsoft Visual Studio 10.0'
    WINSDK_PATH = 'C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A'
    BOOST_PATH = 'C:/boost_1_53_0'

    inc_search_path = [
                       VS_PATH + '/VC/include',
                       BOOST_PATH
    ]

    if int(env['ARCH']) == 32:
        lib_search_path = [
        				   VS_PATH + '/VC/lib',
        				   WINSDK_PATH + '/Lib',
         				   BOOST_PATH + '/lib/win32'
        ]
        
        tool_path = [VS_PATH+'/Common7/IDE',VS_PATH+'/VC/bin'];
        
        
    elif int(env['ARCH']) == 64:
        lib_search_path = [
        				   VS_PATH + '/VC/lib/amd64',
        				   WINSDK_PATH + '/Lib/amd64',
                           BOOST_PATH + '/lib/x64'
        ]
        
        tool_path = [VS_PATH+'/Common7/IDE',VS_PATH+'/VC/bin/x86_amd64'];
        
    else:
        raise Exception, 'Invalid architecture (ARCH)'

    #set search path    
    env['ENV']['PATH'] = tool_path
    
    #set temp directory
    env['ENV']['TMP'] = os.environ['TMP']
    
    #include path
    env.Append(CPPPATH = inc_search_path)
    env.Append(LIBPATH = lib_search_path)
    
    # /Zc:wchar_t-
    # Win-XP
    env['CXXFLAGS'] = "/EHs /MD /D_WIN32_WINNT=0x0501 " 
    # Win-6
    # env['CXXFLAGS'] = "/EHs /MD /D_WIN32_WINNT=0x0601 " 
   
    return env

