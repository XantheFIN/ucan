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

#ifndef LIB_NAME_H_
#define  LIB_NAME_H_

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
#define LIB_NAME(L) (std::string("libC5")+std::string(L)+std::string(".so")).c_str()
#endif

#endif  //LIB_NAME_H_
