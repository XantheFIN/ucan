/*
 * This file is part of a CODESKIN library that is being made available
 * as open source under the GNU Lesser General Public License.
 *
 * Copyright 2005-2018 by CodeSkin LLC, www.codeskin.com.
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

#ifndef LIB_NAME_H_
#define  LIB_NAME_H_

#define LIB_NAME_STRINGIZE(x) #x
#define LIB_NAME_STRINGIZE_VALUE_OF(x) LIB_NAME_STRINGIZE(x)

#include <string>

#ifdef SCONS_TARGET_WIN
#if _WIN64
#define LIB_NAME(L) (std::string("C5")+std::string(L)+std::string("-md_64.dll")).c_str()
#else
#define LIB_NAME(L) (std::string("C5")+std::string(L)+std::string("-md_32.dll")).c_str()
#endif
#endif

#ifdef SCONS_TARGET_MAC
#define LIB_NAME(L) (std::string("libC5")+std::string(L)+std::string(".dylib")).c_str()
#endif

#ifdef SCONS_TARGET_LINUX
#ifdef SCONS_TARGET_CROSS
#define LIB_NAME(L) (std::string("libC5")+std::string(L)+std::string("_")+std::string(LIB_NAME_STRINGIZE_VALUE_OF(SCONS_TARGET_CROSS))+std::string(".so")).c_str()
#else
#define LIB_NAME(L) (std::string("libC5")+std::string(L)+std::string(".so")).c_str()
#endif
#endif

#endif  //LIB_NAME_H_
