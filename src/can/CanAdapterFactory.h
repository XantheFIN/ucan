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

#ifndef CAN_ADAPTER_FACTORY_H_
#define CAN_ADAPTER_FACTORY_H_

#include "CanAdapter.h"

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

DLLEXPORT CanAdapter* createCanAdapterInstance(std::string aChannelName);

DLLEXPORT bool getFirstChannelName(std::string &aName);

DLLEXPORT bool getNextChannelName(std::string &aName);

#ifdef __cplusplus
}
#endif

#endif /* CAN_ADAPTER_FACTORY_H_ */
