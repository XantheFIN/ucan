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

#include <string.h>
#include <sstream>
#include <fstream>

#include <boost/scoped_ptr.hpp>

#include "../can_dll/CanDllPort.hpp"
#include "can_lua.h"

static boost::scoped_ptr<CanDllWrapper> Can;

static void loadWrapper(lua_State *L){
	if(Can == 0){
		Can.reset(new CanDllWrapper());
		if(Can->loadDll() != 0){
			Can.reset();
			luaL_error(L, "Unable to load Can Wrapper!");
		}
	}
}

static int l_init_adapter_type(lua_State *L){
	loadWrapper(L);

	// first argument must be adapter type
	CAN_AdapterType adapter = (CAN_AdapterType)luaL_checkinteger(L, 1);

	// return list of adapters
	lua_newtable(L);
	int i=1;
	char name[100];
	if(Can->getFirstChannelName(adapter, &name[0], 100)){
		lua_pushinteger(L, i++);
		lua_pushstring(L, name);
		lua_settable(L, -3);
		while(Can->getNextChannelName(adapter, &name[0], 100)){
			lua_pushinteger(L, i++);
			lua_pushstring(L, name);
			lua_settable(L, -3);
		}
	}
	return 1;
}

static int l_obtain_handle(lua_State *L){
	loadWrapper(L);

	// first argument must be adapter type
	CAN_AdapterType adapter = (CAN_AdapterType)luaL_checkinteger(L, 1);

	// second argument must be adapter name
	const char *port = luaL_checkstring(L, 2);

	int h = Can->obtainHandle(adapter, port);
	lua_pushnumber(L, h);
	return 1;
}

static int l_release_handle(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	Can->releaseHandle(h);
	return 0;
}

static int l_release_all_handles(lua_State *L){
	loadWrapper(L);

	Can->releaseAllHandles();
	return 0;
}

static int l_handle_exists(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);
	lua_pushboolean(L, Can->handleExists(h));
	return 1;
}

static int l_get_error_code(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushnumber(L, Can->getErrorCode(h));
	return 1;
}

static int l_get_error_description(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	// second argument is code
	int c = luaL_checkinteger(L, 2);

	int secondaryCode;
	char error[1000];
	Can->getErrorDescription(h, c, &error[0], 1000, &secondaryCode);
	lua_pushstring(L, error);
	return 1;
}

static int l_set_parameter(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	// second argument must be parameter table
	luaL_checktype(L, 2, LUA_TTABLE);

	lua_pushnil(L); // first key
	while(lua_next(L, 2) != 0){
		// uses key at index -2 and value at index -1
		const char *k = luaL_checkstring(L, -2);
		const char *v = luaL_checkstring(L, -1);
		if(!Can->setParameter(h, k, v)){
			std::ostringstream errorMsg;
			errorMsg << "Unable to set parameter: " << std::string(k) << "/" << std::string(v);
			luaL_error(L, errorMsg.str().c_str());
			return 0;
		}
		lua_pop(L, 1); // remove value, keep "key" for next iteration
	}
	return 1;
}

static int l_set_baudrate(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	// first argument is baudrate
	int baudrate = luaL_checkinteger(L, 2);

	lua_pushboolean(L, Can->setBaudRate(h, baudrate));
	return 1;
}

static int l_get_number_of_filters(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushnumber(L, Can->getNumberOfFilters(h));
	return 1;
}

static int l_set_acceptance_filter(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);
	// other arguments
	int fid = luaL_checkinteger(L, 2);
	uint32_t code = luaL_checkinteger(L, 3);
	uint32_t mask = luaL_checkinteger(L, 4);
	bool isExt = lua_toboolean(L, 5);

	if(isExt){
		lua_pushboolean(L, Can->setAcceptanceFilter(h, fid, code, mask, 1));
	} else {
		lua_pushboolean(L, Can->setAcceptanceFilter(h, fid, code, mask, 0));
	}
	return 1;
}

static int l_open(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushboolean(L, Can->open(h));
	return 1;
}

static int l_close(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	Can->close(h);
	return 0;
}

static int l_go_bus_on(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushboolean(L, Can->goBusOn(h));
	return 1;
}

static int l_go_bus_off(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushboolean(L, Can->goBusOff(h));
	return 1;
}

static int l_get_state(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushnumber(L, Can->getState(h));
	return 1;
}

static int l_get_error_counters(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	int txErrorCounter = 0;
	int rxErrorCounter = 0;
	Can->getErrorCounters(h, &txErrorCounter, &rxErrorCounter);
	lua_pushnumber(L, txErrorCounter);
	lua_pushnumber(L, rxErrorCounter);
	return 2;
}

static int l_num_received_messages_available(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushnumber(L, Can->numReceivedMessagesAvailable(h));
	return 1;
}

static int l_num_sent_messages_available(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	lua_pushnumber(L, Can->numSentMessagesAvailable(h));
	return 1;
}

static int l_send_message(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);

	// second argument must be parameter table
	luaL_checktype(L, 2, LUA_TTABLE);

	lua_pushstring(L, "id");
	lua_gettable(L, -2);
	uint32_t id = luaL_checkinteger(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "data");
	lua_gettable(L, -2);
	const char *dataC = luaL_checkstring(L, -1);
	std::string data(dataC);
	lua_pop(L, 1);

	lua_pushstring(L, "isext");
	lua_gettable(L, -2);
	uint16_t isExt = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if((data.length() & 1) != 0){
		return luaL_error(L, "Invalid record.");
	}

	unsigned int len = data.length()/2;
	if(len > 8){
		return luaL_error(L, "Record too long.");
	}

	CAN_CanMessage m;
	m.id = id;
	m.len = len;
	m.flags = 0;
	if(isExt){
		m.flags |= CAN_FLAG_IS_EXTENDED;
	}
	for(int i=0; i<len; i++){
		int b = (int)strtol(data.substr(i*2,2).c_str(), NULL, 16);
		m.data[i] = b;
	}

	lua_pushboolean(L, Can->sendMessage(h, &m, 0));
	return 1;
}

static int l_get_sent_message(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);
	// timeout
	uint32_t timeout = luaL_checkinteger(L, 2);

	CAN_CanMessage m;
	if(!Can->getSentMessage(h, &m, timeout)){
		lua_pushnil(L);
		return 1;
	}

	std::ostringstream oss;
	for(int i=0; i<m.len; i++){
		oss <<  std::hex << std::setw(2) << std::setfill('0') << ((unsigned int)m.data[i]);
	}

	// return message
	lua_newtable(L);
	lua_pushstring(L, "id");
	lua_pushnumber(L, m.id);
	lua_settable(L, -3);
	lua_pushstring(L, "isext");
	lua_pushboolean(L, ((m.flags & CAN_FLAG_IS_EXTENDED) != 0));
	lua_settable(L, -3);
	lua_pushstring(L, "data");
	lua_pushstring(L, oss.str().c_str());
	lua_settable(L, -3);
	return 1;
}

static int l_get_received_message(lua_State *L){
	loadWrapper(L);

	// first argument must be handle
	int h = luaL_checkinteger(L, 1);
	// timeout
	uint32_t timeout = luaL_checkinteger(L, 2);

	CAN_CanMessage m;
	if(!Can->getReceivedMessage(h, &m, timeout)){
		lua_pushnil(L);
		return 1;
	}

	std::ostringstream oss;
	for(int i=0; i<m.len; i++){
		oss <<  std::hex << std::setw(2) << std::setfill('0') << ((unsigned int)m.data[i]);
	}

	// return message
	lua_newtable(L);
	lua_pushstring(L, "id");
	lua_pushnumber(L, m.id);
	lua_settable(L, -3);
	lua_pushstring(L, "isext");
	lua_pushboolean(L, ((m.flags & CAN_FLAG_IS_EXTENDED) != 0));
	lua_settable(L, -3);
	lua_pushstring(L, "data");
	lua_pushstring(L, oss.str().c_str());
	lua_settable(L, -3);
	return 1;
}

static const struct luaL_Reg mylib[]={
		{"init_adapter_type", l_init_adapter_type},
		{"obtain_handle", l_obtain_handle},
		{"handle_exists", l_handle_exists},
		{"get_error_code", l_get_error_code},
		{"get_error_description", l_get_error_description},
		{"release_handle", l_release_handle},
		{"release_all_handles", l_release_all_handles},
		{"set_parameter", l_set_parameter},
		{"set_baudrate", l_set_baudrate},
		{"get_number_of_filters", l_get_number_of_filters},
		{"set_acceptance_filter", l_set_acceptance_filter},
		{"open", l_open},
		{"close", l_close},
		{"go_bus_on", l_go_bus_on},
		{"go_bus_off", l_go_bus_off},
		{"get_state", l_get_state},
		{"get_error_counters", l_get_error_counters},
		{"num_received_messages_available", l_num_received_messages_available},
		{"num_sent_messages_available", l_num_received_messages_available},
		{"send_message", l_send_message},
		{"get_sent_message", l_get_sent_message},
		{"get_received_message", l_get_received_message},
		{NULL, NULL}
};

int luaopen_CanLua(lua_State *L){
	luaL_newlib(L, mylib);
	lua_pushstring(L, "Echo");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);
	lua_pushstring(L, "Ni");
	lua_pushnumber(L, 1);
	lua_settable(L, -3);
	lua_pushstring(L, "Lawicel");
	lua_pushnumber(L, 2);
	lua_settable(L, -3);
	lua_pushstring(L, "Peak");
	lua_pushnumber(L, 3);
	lua_settable(L, -3);
	lua_pushstring(L, "Kvaser");
	lua_pushnumber(L, 4);
	lua_settable(L, -3);
	lua_pushstring(L, "Vector");
	lua_pushnumber(L, 5);
	lua_settable(L, -3);
	lua_pushstring(L, "EmsWuensche");
	lua_pushnumber(L, 6);
	lua_settable(L, -3);
	lua_pushstring(L, "SLCan");
	lua_pushnumber(L, 7);
	lua_settable(L, -3);
	lua_pushstring(L, "SocketCan");
	lua_pushnumber(L, 8);
	lua_settable(L, -3);
	lua_pushstring(L, "NiXnetCan");
	lua_pushnumber(L, 9);
	lua_settable(L, -3);
	return 1;
}



int CANLUA_runScriptForHandle(const char *aFileName, int aHandle){
	std::string fileNameString(aFileName);
	std::ifstream t(fileNameString);
	std::string lua_script((std::istreambuf_iterator<char>(t)),
	                 std::istreambuf_iterator<char>());
	int error;
	// create LUA state and load standard libraries
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	// set path to this custom module
    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "cpath" ); // get field "path" from table at top of stack (-1)
    std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack
#if _WIN64
    cur_path.append(";./build/can_lua/test/C5?-md_64.dll");
#else
    cur_path.append(";./build/can_lua/test/C5?-md_32.dll");
#endif
    lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
    lua_pushstring( L, cur_path.c_str() ); // push the new one
    lua_setfield( L, -2, "cpath" ); // set the field "path" in table at -2 with value at top of stack
    lua_pop( L, 1 ); // get rid of package table from top of stack

	// now load this module and save as global variable "Can"
	lua_getglobal(L, "require");  // function
	lua_pushstring (L, "CanLua");     // arg 0: module name
	error = lua_pcall (L, 1, 1, 0);  // 1 arg, return 1 result
	if(error){
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		return error;
	}
	lua_setglobal(L, "can");

	// push handle into global variable "can_h"
	lua_pushnumber (L, aHandle);
	lua_setglobal(L, "can_h");

	// finally, run script
	error = luaL_loadstring(L, lua_script.c_str()) || lua_pcall(L, 0, 0, 0);
	if(error){
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		lua_pop(L,1);
	}

	lua_close(L);
    return error;
}

int CANLUA_runScript(const char *aFileName){
	return CANLUA_runScriptForHandle(aFileName, 0);
}

