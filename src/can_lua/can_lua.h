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

#ifndef CAN_LUA_H_
#define CAN_LUA_H_

#if defined _WIN32
  #define DLLEXPORT __declspec(dllexport)
#elif __GNUC__ >=4
  #define DLLEXPORT __attribute__ ((visibility("default")))
#else
  #define DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

DLLEXPORT int luaopen_CanLua(lua_State *L);
DLLEXPORT int CANLUA_runScript(const char *aFileName);
DLLEXPORT int CANLUA_runScriptForHandle(const char *aFileName, int aHandle);

#ifdef __cplusplus
}
#endif

#endif /* CAN_LUA_H_ */
