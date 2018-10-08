from SCons.Util import *
import os
import sys

def set_build_options(env):
	env.Append(CCFLAGS = ['-DSCONS_TARGET_UNIX'])
	env.Append(CCFLAGS = ['-DSCONS_TARGET_LINUX'])
	env.Append(CCFLAGS = ['-DSCONS_TARGET_CROSS=armhf'])	
		
	xc_prefix = 'arm-linux-gnueabihf-'
	env['CC'] = xc_prefix + 'gcc'
	env['CXX'] = xc_prefix + 'gcc'
	env['AR'] = xc_prefix + 'ar'
	env['RANLIB'] = 'xc_prefix + 'ranlib'
	
	home = os.path.expanduser("~")
	env['XGCC_PATH'] = home+'/opt/gcc-linaro-6.4.1-2018.05-x86_64_arm-linux-gnueabihf'
	env['XSYS_ROOT'] = home+'/opt/sysroot-glibc-linaro-2.23-2018.05-arm-linux-gnueabihf'

	inc_search_path = [
		home+'/lib/armhf/boost_1_53_0/include'
	]
	
	lib_search_path = [
		home+'/lib/armhf/boost_1_53_0/lib'
	]

    #augment search path
	env.PrependENVPath('PATH', [
		env['XGCC_PATH']+'/bin'
    	]);
		
    #include path
	env.Append(CPPPATH = inc_search_path)
	env.Append(LIBPATH = lib_search_path)  
		        
	#-pedantic-errors	        
	env.Append(CXXFLAGS = ['-g','-std=c++11'])	
	env.Append(CFLAGS = ['-g'])
	
	env.Append(LINKFLAGS= ['-lstdc++','-shared-libgcc']) 

	return env
