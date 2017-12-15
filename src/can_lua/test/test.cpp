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

#include <iostream>
#include <boost/scope_exit.hpp>

#include "CanLuaWrapper.hpp"
#include "CanDllWrapper.hpp"

int testWithHandle(){
	CanDllWrapper can;
	if(can.loadDll() != 0)
	{
		std::cout << "Unable to load can!!!" << std::endl;
		return EXIT_FAILURE;
	}

	CAN_AdapterType type = CAN_NiXnetCan;

	std::cout << "List of adapters detected:" << std::endl;
	char name[100];
	if(can.getFirstChannelName(type, &name[0], 100)){
		std::cout << " * " << name << std::endl;
		while(can.getNextChannelName(type, &name[0], 100)){
			std::cout << " * " << name << std::endl;
		}
	}
	std::cout << std::flush;

	int handle = 0;
	BOOST_SCOPE_EXIT(&can, &handle)
	{
		if(handle != 0){
			can.releaseHandle(handle);
			handle = 0;
			std::cout << "Can disconnected and handle released." << std::endl;
		}
		can.unloadDll();
		std::cout << "Dll unloaded." << std::endl;
	} BOOST_SCOPE_EXIT_END;

	handle = can.obtainHandle(type, "1");
	if(handle == 0){
		std::cout << "Unable to get handle." << std::endl;
		return EXIT_FAILURE;
	}

	CanLuaWrapper CanLua;

	if(CanLua.loadDll() != 0){
		std::cout << "Unable to load CanDll Wrapper!" << std::endl;
		return 0;
	}

	return CanLua.runScriptForHandle("build/can_lua/test/test.lua", handle);
}

int test(){
	CanLuaWrapper CanLua;

	if(CanLua.loadDll() != 0){
		std::cout << "Unable to load CanDll Wrapper!" << std::endl;
		return 0;
	}

	return CanLua.runScript("build/can_lua/test/test.lua");
}

int main(int argc, char* argv[]){
	test();
}
