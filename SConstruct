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

"""
Top-level SCons Build File
"""

import os
import sys
import platform

# ensure these versions of tools or newer:
EnsurePythonVersion(2,6)
EnsureSConsVersion(1,2)

# get build options
vars = Variables('build_options.py') 
vars.AddVariables(
        ('ARCH', 'Architecture (32/64bit)','32'),
        ('RELEASE', 'Set to 1 to build for release', '0'),
        ('INSTALL_DIRS', 'Specify installation directories', []),    
        )
   
# create new environment
env = Environment(variables = vars);

unknown = vars.UnknownVariables()
if unknown:
	print("Unknown variables:", unknown.keys())
	Exit(1)

Help(vars.GenerateHelpText(env))

# configure toolchain
if((os.name == 'posix') and (platform.system() == 'Darwin' )):
    import mac_build
    mac_build.set_build_options(env)
    
elif ((os.name == 'nt') and (platform.system() == 'Windows' )):
    import vs_build
    vs_build.set_build_options(env)
    
elif ((os.name == 'posix') and (platform.system() == 'Linux' )):
    import linux_build
    linux_build.set_build_options(env) 
    
else:
     raise Exception, 'Don''t know this os and platform ('+os.name+', '+platform.system()+')'

# add libpath tool
import libname
env.AddMethod(libname.lib_name, "LibName")
env.AddMethod(libname.sharedlib_name, "SharedLibName")
env.AddMethod(libname.sharedlib_name_with_ext, "SharedLibNameExt")
env.AddMethod(libname.external_sharedlib_name_with_ext, "ExtDll")

# export for hierarchical build
Export('env');

# we build in variant directory (as to separate source from generated code)
VariantDir('build/utils', 'src/utils', 0)
VariantDir('build/can', 'src/can', 0)
VariantDir('build/can_dll', 'src/can_dll', 0)
VariantDir('build/can_async', 'src/can_async', 0)
VariantDir('build/can_lua', 'src/can_lua', 0)
VariantDir('build/slcan_can', 'src/slcan_can', 0)
VariantDir('build/socketcan_can', 'src/socketcan_can', 0)

# subdirectories needing to be built 
if ((os.name == 'nt') and (platform.system() == 'Windows' )):
	if int(env['ARCH']) == 32:
		srcs = [
		'build/utils',
		'build/can',
		'build/can_dll',
		'build/can_lua',
		'build/can_async',
		'build/slcan_can',
		]
	else:
		srcs = [
		'build/utils',
		'build/can',
		'build/can_dll',
		'build/can_lua',
		'build/can_async',
		'build/slcan_can',
		]
	
elif ((os.name == 'posix') and (platform.system() == 'Linux' )):
    srcs = [
    'build/utils',
    'build/can',
    'build/can_dll',
    'build/can_lua',
    'build/can_async',
	'build/slcan_can',
    'build/socketcan_can'
    ]
            
elif ((os.name == 'posix') and (platform.system() == 'Darwin' )):
	srcs = [
	'build/utils',
	'build/can',
	'build/can_dll',
    'build/can_lua',
	'build/can_async',
	'build/slcan_can'
	]

else:
     srcs = []

# built 
objs = []
for subdir in srcs:
    o = SConscript('%s/SConscript' % subdir)
    objs.append(o)
